#include <iostream>
#include <Windows.h>
#include <sstream>
#include <thread>
#include "CommandHandler.h"
#include "PathParser.h"
#include "StringUtility.h"
#include "Memory.h"
#include "Aimbot.h"
#include "Render.h"

void CommandHandler::Start(HANDLE handle, HWND hwnd, DWORD game, DWORD playersService, DWORD localPlayer, DWORD visualEngine) {
    PathParser pathParser(game);

    Render render(handle, playersService, localPlayer, visualEngine);
    render.EnableESP();
    render.SetCrosshair(true);

    Aimbot aimbot(hwnd, playersService, localPlayer);
    std::thread aimbotThread(&Aimbot::Start, aimbot);

    std::cout << "Command handler is ready to start receiving commands." << std::endl;

    while (true) {
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);

        std::vector<std::string> split = StringUtility::Split(command, ' ');
        std::string request = split.front();
        request = StringUtility::ToLowerCase(request);
        
        if (request == "get") {
            std::string path = split.back();
            std::cout << std::hex << pathParser.ParsePath(path) << std::dec << std::endl;
        } else if (request == "getpos") {
            DWORD character = Memory::GetCharacter(localPlayer);
            DWORD humanoidRootPart = Memory::FindFirstChild(character, "HumanoidRootPart");
            Vector3 position = Memory::GetPosition(humanoidRootPart);
            std::cout << position.X << " " << position.Y << " " << position.Z << std::endl;
        } else if (request == "setpos") {
            DWORD character = Memory::GetCharacter(localPlayer);
            DWORD humanoidRootPart = Memory::FindFirstChild(character, "HumanoidRootPart");
            Vector3 currentPosition = Memory::GetPosition(humanoidRootPart);

            std::string x = split.at(1);
            std::string y = split.at(2);
            std::string z = split.at(3);

            if (StringUtility::ToLowerCase(x) == "x") {
                x = std::to_string(currentPosition.X);
            }
            if (StringUtility::ToLowerCase(y) == "y") {
                y = std::to_string(currentPosition.Y);
            }
            if (StringUtility::ToLowerCase(z) == "z") {
                z = std::to_string(currentPosition.Z);
            }

            Vector3 targetPosition = {std::stof(x), std::stof(y), std::stof(z)};
            Memory::SetPosition(humanoidRootPart, targetPosition);

            std::cout << "Set position to " << targetPosition.X << " " << targetPosition.Y << " " << targetPosition.Z << std::endl;
        } else if (request == "aimbot") {
            std::string option = StringUtility::ToLowerCase(split.back());
            if (option == "on") {
                aimbotThread = std::thread(&Aimbot::Start, aimbot);
                std::cout << "Aimbot enabled." << std::endl;
            } else if (option == "off") {
                aimbot.Stop();
                aimbotThread.join();
                std::cout << "Aimbot disabled." << std::endl;
            }
        } else if (request == "esp") {
            std::string option = StringUtility::ToLowerCase(split.back());
            if (option == "on") {
                render.EnableESP();
                std::cout << "ESP enabled." << std::endl;
            } else if (option == "off") {
                render.DisableESP();
                std::cout << "ESP disabled." << std::endl;
            }
        } else if (request == "teamcheck") {
            std::string option = StringUtility::ToLowerCase(split.back());
            if (option == "on") {
                render.SetTeamCheck(true);
                aimbot.SetTeamCheck(true);
                std::cout << "Team check enabled." << std::endl;
            } else if (option == "off") {
                render.SetTeamCheck(false);
                aimbot.SetTeamCheck(false);
                std::cout << "Team check disabled." << std::endl;
            }
        } else if (request == "crosshair") {
            std::string option = StringUtility::ToLowerCase(split.back());
            if (option == "on") {
                render.SetCrosshair(true);
                std::cout << "Crosshair enabled." << std::endl;
            } else if (option == "off") {
                render.SetCrosshair(false);
                std::cout << "Crosshair disabled." << std::endl;
            }
        }
    }
}