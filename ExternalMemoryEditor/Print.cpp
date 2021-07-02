#include <iostream>
#include "Print.h"
#include "Addresses.h"
#include "Memory.h"

struct Args {
    int Type;
    const char* Text;

    DWORD Address;
};

void mappedFunction(void* pArgs) {
    typedef void __cdecl _print(int, const char*);
    Args* args = (Args*)pArgs;
    _print* print = (_print*)args->Address;
    print(args->Type, args->Text);
}

void markerFunction() { // Mark the end of the location of 'mappedFunction'
    return;
}

Print::Print(HANDLE handle, DWORD baseAddress) {
    this->Handle = handle;
    this->BaseAddress = baseAddress;

    this->FunctionSize = (uintptr_t)markerFunction - (uintptr_t)mappedFunction;
    this->FunctionAndArgsSize = this->FunctionSize + sizeof(Args);

    this->FunctionAndArgsMemory = VirtualAllocEx(this->Handle, 0, this->FunctionAndArgsSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!this->FunctionAndArgsMemory) {
        std::cout << "Couldn't allocate memory for arguments." << std::endl;
        return;
    }

    //std::cout << std::hex << this->FunctionAndArgsMemory << std::dec << std::endl;
}

void Print::PrintLine(int type, const char* text) {
    const char* stringMemory = (const char*)Memory::CreateCharPointerString(text);

    Args args = { type, stringMemory, this->BaseAddress + Addresses::Print };

    void* argsMemory = (void*)((uintptr_t)this->FunctionAndArgsMemory + this->FunctionSize);

    if (!WriteProcessMemory(this->Handle, this->FunctionAndArgsMemory, mappedFunction, this->FunctionSize, 0) || !WriteProcessMemory(this->Handle, argsMemory, &args, sizeof(args), 0)) {
        std::cout << "Couldn't write function and/or arguments." << std::endl;
        return;
    }

    HANDLE thread = CreateRemoteThread(this->Handle, 0, 0, (LPTHREAD_START_ROUTINE)this->FunctionAndArgsMemory, argsMemory, 0, 0);
    if (!thread) {
        std::cout << "Failed to create thread." << std::endl;
        return;
    }
    WaitForSingleObject(thread, INFINITE);

    //VirtualFreeEx(this->Handle, this->FunctionAndArgsMemory, 0, MEM_RELEASE);
    CloseHandle(thread);
}