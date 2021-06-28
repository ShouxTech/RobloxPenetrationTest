#pragma once
#include <vector>

class Memory {
	public:
		static DWORD GetProcessID(const wchar_t* processName);

		static uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

		static DWORD Scan(HANDLE handle, DWORD baseAddress, DWORD VFTableAddress);

		template <typename T>
		static T Read(HANDLE handle, LPCVOID address, SIZE_T size = sizeof(T));

		static DWORD GetPointerAddress(HANDLE handle, DWORD address);

		static uintptr_t GetDMAAddress(HANDLE handle, uintptr_t ptr, std::vector<unsigned int> offsets);

		static std::string ReadStringOfUnknownLength(HANDLE handle, DWORD address);

		static void* CreateCharPointerString(HANDLE handle, const char* string);

		static std::string GetName(HANDLE handle, DWORD instance);

		static std::vector<DWORD> GetChildren(HANDLE handle, DWORD instance);

		static DWORD FindFirstChild(HANDLE handle, DWORD instance, std::string name);
};