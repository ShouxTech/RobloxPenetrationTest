#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"
#include "Addresses.h"
#include "Print.h"
#include "PathParser.h"
#include "LuaStateHook.h"

int main() {
    HWND hwnd = FindWindowA(NULL, "Roblox");
    if (!hwnd) {
        std::cout << "Couldn't find Roblox." << std::endl;
        return 0;
    }

    DWORD processID;
    HANDLE handle;

    GetWindowThreadProcessId(hwnd, &processID);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

    if (!processID || !handle) {
        std::cout << "Couldn't attach to Roblox." << std::endl;
        return 0;
    }

    long long placeID = 0;
    long long userID = 0;
    char jobID[37];
    char username[21];
    std::string characterAppearanceID;
    std::string connectionLocation;
    std::string teamName;

    uintptr_t baseAddress = Memory::GetModuleBaseAddress(processID, L"RobloxPlayerBeta.exe");

    std::cout << "Scanning..." << std::endl;

    DWORD game = Memory::Scan(handle, baseAddress, baseAddress + Addresses::DataModel);
    DWORD players = Memory::FindFirstChild(handle, game, "Players");
    DWORD localPlayer = Memory::GetPointerAddress(handle, players + 0x110);
    DWORD character = Memory::GetPointerAddress(handle, localPlayer + 0x64);
    DWORD team = Memory::GetPointerAddress(handle, localPlayer + 0x98);

    ReadProcessMemory(handle, (LPCVOID)(baseAddress + Addresses::PlaceID), &placeID, sizeof(placeID), 0);

    ReadProcessMemory(handle, (LPCVOID)(localPlayer + 0xF8), &userID, sizeof(userID), 0);

    ReadProcessMemory(handle, (LPCVOID)Memory::GetPointerAddress(handle, game + 0x284), &jobID, sizeof(jobID), 0);

    ReadProcessMemory(handle, (LPCVOID)Memory::GetPointerAddress(handle, localPlayer + 0x28), &username, sizeof(username), 0);

    characterAppearanceID = Memory::ReadStringOfUnknownLength(handle, Memory::GetPointerAddress(handle, localPlayer + 0x170));

    connectionLocation = Memory::ReadStringOfUnknownLength(handle, Memory::GetPointerAddress(handle, game + 0x314));

    if (team) {
        teamName = Memory::GetName(handle, team);
    } else {
        teamName = "None";
    }
    
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Base address: " << std::hex << baseAddress << std::dec << std::endl;
    std::cout << "DataModel address: " << std::hex << game << std::dec << std::endl;
    std::cout << "Players address: " << std::hex << players << std::dec << std::endl;
    std::cout << "LocalPlayer address: " << std::hex << localPlayer << std::dec << std::endl;
    std::cout << "Character address: " << std::hex << character << std::dec << std::endl;
    std::cout << "Index2Adr address: " << std::hex << baseAddress + Addresses::Index2Adr << std::dec << std::endl;
    //std::cout << "HumanoidRootPart address: " << std::hex << Memory::FindFirstChild(handle, character, "HumanoidRootPart") << std::dec << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    std::cout << "Place ID: " << placeID << std::endl;
    std::cout << "User ID: " << userID << std::endl;
    std::cout << "Job ID: " << jobID << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Character appearance ID: " << characterAppearanceID << std::endl;
    std::cout << "Team: " << teamName << std::endl;
    std::cout << "Connection location (serverIP|port): " << connectionLocation << std::endl;

    DWORD luaState = LuaStateHook::Hook(handle, baseAddress);

    Print print(handle, baseAddress);
    print.PrintLine(0, "Hi");
    print.PrintLine(1, "Bye");
    print.PrintLine(2, "Thx");
    print.PrintLine(3, "Np");

    PathParser pathParser(handle, game);

    while (true) {
        std::string path;
        std::cout << "> ";
        std::cin >> path;
        std::cout << std::hex << pathParser.ParsePath(path) << std::dec << std::endl;
    }

    //system("pause");
}