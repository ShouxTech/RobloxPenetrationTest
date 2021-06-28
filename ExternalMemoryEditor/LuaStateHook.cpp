#include <iostream>
#include "LuaStateHook.h"
#include "Addresses.h"

void loadHookBytes(HANDLE handle, void* address, void* assemblyHook, unsigned int overwriteBytesAmount) {
    if (overwriteBytesAmount < 5) {
        std::cout << "Not enough bytes to overwrite. JMP requires 5 bytes." << std::endl;
        return;
    }

    DWORD oldProtection;
    if (!VirtualProtectEx(handle, address, overwriteBytesAmount, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        std::cout << "Couldn't change memory protection." << std::endl;
        return;
    }

    DWORD relativeAddress = (DWORD)assemblyHook - ((DWORD)address + 5);

    char* buffer = new char[overwriteBytesAmount];
    memset(buffer, 0x90, overwriteBytesAmount);
    buffer[0] = (char)0xE9;
    memcpy(&buffer[1], &relativeAddress, sizeof(relativeAddress));

    WriteProcessMemory(handle, address, buffer, overwriteBytesAmount, nullptr);

    DWORD temp;
    VirtualProtectEx(handle, address, overwriteBytesAmount, oldProtection, &temp);

    delete[] buffer;
}

DWORD LuaStateHook::Hook(HANDLE handle, DWORD baseAddress) {
    DWORD index2adr = baseAddress + Addresses::Index2Adr;

    int hookSize = 6;

    /*
        mov eax, [ebp + 0xC]
        push [eax + 0x4]
        jmp 0
        https://defuse.ca/online-x86-assembler.htm
    */
    BYTE assemblyHook[11] = {
        0x8B, 0x45, 0x0C,
        0xFF, 0x70, 0x04,
        0xE9, 0x0, 0x0, 0x0, 0x0
    };
    int jmpBackOffset = 7;

    void* hookLocation = VirtualAllocEx(handle, 0, sizeof(assemblyHook), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    DWORD jmpBackAddress = (index2adr + hookSize) - ((DWORD)hookLocation + sizeof(assemblyHook));
    memcpy(&assemblyHook[jmpBackOffset], (void*)&jmpBackAddress, sizeof(jmpBackAddress));

    WriteProcessMemory(handle, hookLocation, assemblyHook, sizeof(assemblyHook), 0);

    BYTE originalBytes[6]; // 6 = hookSize
    ReadProcessMemory(handle, (LPCVOID)index2adr, originalBytes, sizeof(originalBytes), 0);

    loadHookBytes(handle, (void*)index2adr, hookLocation, hookSize);
    Sleep(100);
    WriteProcessMemory(handle, (LPVOID)index2adr, originalBytes, sizeof(originalBytes), 0);

	return 0;
}