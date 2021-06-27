#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Memory.h"

uintptr_t Memory::GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

DWORD Memory::Scan(HANDLE handle, DWORD baseAddress, DWORD VFTableAddress) {
	SYSTEM_INFO systemInfo;
	DWORD pageSize;
	DWORD pageSize4ByteSplit;
	MEMORY_BASIC_INFORMATION memoryInfo;
	GetSystemInfo(&systemInfo);
	pageSize = systemInfo.dwPageSize;
	pageSize4ByteSplit = pageSize / 4;
	DWORD* buffer = new DWORD[pageSize];
	
	for (DWORD addr = baseAddress; addr < 0x7FFFFFFF; addr += pageSize) {
		VirtualQueryEx(handle, (LPCVOID)addr, &memoryInfo, pageSize);
		if (memoryInfo.Protect == PAGE_READWRITE) {
			ReadProcessMemory(handle, (LPCVOID)addr, buffer, pageSize, 0);
			for (DWORD i = 0; i <= pageSize4ByteSplit; i++) {
				if (buffer[i] == VFTableAddress) {
					delete[] buffer;
					return (DWORD)(addr + (i * 4));
				}
			}
		}
	}

	delete[] buffer;
	return 0;
}

template<typename T>
T Memory::Read(HANDLE handle, LPCVOID address, SIZE_T size) {
	T buffer;
	ReadProcessMemory(handle, address, &buffer, size, 0);
	return buffer;
}

DWORD Memory::GetPointerAddress(HANDLE handle, DWORD address) {
	uintptr_t pointerAddress = GetDMAAddress(handle, address, { 0x0 });
	return pointerAddress;
}

uintptr_t Memory::GetDMAAddress(HANDLE handle, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		addr = Read<uintptr_t>(handle, (LPCVOID)addr);
		//ReadProcessMemory(handle, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

std::string Memory::ReadStringOfUnknownLength(HANDLE handle, DWORD address) {
	std::string string;
	char character = 0;
	int charSize = sizeof(character);
	int offset = 0;

	while (true) {
		character = Read<char>(handle, (LPCVOID)(address + offset));
		if (character == 0) break;
		offset += charSize;
		string.push_back(character);
	}

	return string;
}

void* Memory::CreateCharPointerString(HANDLE handle, const char* string) {
	void* stringMemory = VirtualAllocEx(handle, 0, strlen(string), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!stringMemory) {
		std::cout << "Couldn't allocate memory for \"" << string << "\" char pointer." << std::endl;
		return 0;
	}

	if (!WriteProcessMemory(handle, stringMemory, string, strlen(string), 0)) {
		std::cout << "Couldn't write \"" << string << "\" to memory." << std::endl;
		return 0;
	}

	return stringMemory;
}

std::string Memory::GetName(HANDLE handle, DWORD instance) {
	uintptr_t nameAddress = GetPointerAddress(handle, instance + 0x28);
	std::string name = ReadStringOfUnknownLength(handle, nameAddress);

	int size = Read<int>(handle, (LPCVOID)(nameAddress + 0x14));

	if (size >= 16u) {
		uintptr_t newNameAddress = GetPointerAddress(handle, nameAddress);
		return ReadStringOfUnknownLength(handle, newNameAddress);
	} else {
		return name;
	}
}

std::vector<DWORD> Memory::GetChildren(HANDLE handle, DWORD instance) {
	std::vector<DWORD> children;

	DWORD v4 = GetPointerAddress(handle, instance + 0x2C);
	int v25 = (GetPointerAddress(handle, v4 + 4) - GetPointerAddress(handle, v4)) >> 3;
	if (!v25) {
		std::cout << "Couldn't get number of children." << std::endl;
		return children;
	}

	DWORD v6 = GetPointerAddress(handle, v4);
	for (int i = 0; i < v25; i++) {
		children.push_back(GetPointerAddress(handle, v6));
		v6 += 8;
	}

	return children;
}

DWORD Memory::FindFirstChild(HANDLE handle, DWORD instance, std::string name) {
	std::vector<DWORD> children = GetChildren(handle, instance);

	for (DWORD child : children) {
		if (GetName(handle, child) == name) {
			return child;
		}
	}
}