#pragma once
#include <Windows.h>

class Print {
    private:
        HANDLE Handle;
        DWORD BaseAddress;
        uintptr_t FunctionSize;
        uintptr_t FunctionAndArgsSize;
        void* FunctionAndArgsMemory;
    public:
        Print(HANDLE handle, DWORD baseAddress);
        void PrintLine(int type, const char* text);
};
