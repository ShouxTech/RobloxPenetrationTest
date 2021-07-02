#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"
#include "Addresses.h"
#include "Print.h"
//#include "LuaStateHook.h"
#include "CommandHandler.h"
#include "Utility.h"

int main() {
    SetConsoleTitle((LPCWSTR)Utility::CreateRandomString(6).c_str());

    DWORD processID = Memory::GetProcessID(L"RobloxPlayerBeta.exe");
    if (!processID) {
        std::cout << "Couldn't get Roblox process ID." << std::endl;
        return 0;
    }

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (!handle) {
        std::cout << "Couldn't get Roblox handle." << std::endl;
        return 0;
    }
    Memory::SetHandle(handle);

    HWND hwnd = FindWindowA(NULL, "Roblox");

    long long placeID = 0;
    long long userID = 0;
    char jobID[37];
    char username[21];
    std::string characterAppearanceID;
    std::string connectionLocation;
    std::string teamName;

    uintptr_t baseAddress = Memory::GetModuleBaseAddress(processID, L"RobloxPlayerBeta.exe");

    std::cout << "Scanning..." << std::endl;

    DWORD game = Memory::Scan(baseAddress, baseAddress + Addresses::DataModel);
    DWORD players = Memory::GetService(game, "Players");
    DWORD localPlayer = Memory::GetPointerAddress(players + 0x110);
    DWORD character = Memory::GetCharacter(localPlayer);
    DWORD team = Memory::GetTeam(localPlayer);

    DWORD visualEngine = Memory::Scan(baseAddress, baseAddress + Addresses::VisualEngine);

    ReadProcessMemory(handle, (LPCVOID)(baseAddress + Addresses::PlaceID), &placeID, sizeof(placeID), 0);

    ReadProcessMemory(handle, (LPCVOID)(localPlayer + 0xF8), &userID, sizeof(userID), 0);

    ReadProcessMemory(handle, (LPCVOID)Memory::GetPointerAddress(game + 0x284), &jobID, sizeof(jobID), 0);

    ReadProcessMemory(handle, (LPCVOID)Memory::GetPointerAddress(localPlayer + 0x28), &username, sizeof(username), 0);

    characterAppearanceID = Memory::ReadStringOfUnknownLength(Memory::GetPointerAddress(localPlayer + 0x170));

    connectionLocation = Memory::ReadStringOfUnknownLength(Memory::GetPointerAddress(game + 0x314));

    if (team) {
        teamName = Memory::GetName(team);
    } else {
        teamName = "None";
    }
    
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Base address: " << std::hex << baseAddress << std::dec << std::endl;
    std::cout << "DataModel address: " << std::hex << game << std::dec << std::endl;
    std::cout << "Players address: " << std::hex << players << std::dec << std::endl;
    std::cout << "LocalPlayer address: " << std::hex << localPlayer << std::dec << std::endl;
    std::cout << "Character address: " << std::hex << character << std::dec << std::endl;
    std::cout << "VisualEngine address: " << std::hex << visualEngine << std::dec << std::endl;
    //std::cout << "Index2Adr address: " << std::hex << baseAddress + Addresses::Index2Adr << std::dec << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    std::cout << "Place ID: " << placeID << std::endl;
    std::cout << "User ID: " << userID << std::endl;
    std::cout << "Job ID: " << jobID << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Character appearance ID: " << characterAppearanceID << std::endl;
    std::cout << "Team: " << teamName << std::endl;
    std::cout << "Connection location (serverIP|port): " << connectionLocation << std::endl;

    Print print(handle, baseAddress);
    print.PrintLine(0, "Hi");
    print.PrintLine(1, "Bye");
    print.PrintLine(2, "Thx");
    print.PrintLine(3, "Np");

    CommandHandler::Start(handle, hwnd, game, players, localPlayer, visualEngine);

    //system("pause");
}