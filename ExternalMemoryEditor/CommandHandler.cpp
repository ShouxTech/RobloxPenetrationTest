#include <iostream>
#include <Windows.h>
#include <sstream>
#include "CommandHandler.h"
#include "PathParser.h"
#include "StringUtility.h"
#include "Memory.h"

void CommandHandler::Start(HANDLE handle, DWORD game, DWORD localPlayer) {
    PathParser pathParser(handle, game);

    std::cout << "Command handler ready to start receiving commands." << std::endl;

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
            DWORD character = Memory::GetCharacter(handle, localPlayer);
            DWORD humanoidRootPart = Memory::FindFirstChild(handle, character, "HumanoidRootPart");
            Vector3 position = Memory::GetPosition(handle, humanoidRootPart);
            std::cout << position.X << " " << position.Y << " " << position.Z << std::endl;
        } else if (request == "setpos") {
            DWORD character = Memory::GetCharacter(handle, localPlayer);
            DWORD humanoidRootPart = Memory::FindFirstChild(handle, character, "HumanoidRootPart");
            Vector3 currentPosition = Memory::GetPosition(handle, humanoidRootPart);

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
            Memory::SetPosition(handle, humanoidRootPart, targetPosition);

            std::cout << "Set position to " << targetPosition.X << " " << targetPosition.Y << " " << targetPosition.Z << std::endl;
        }
    }
}