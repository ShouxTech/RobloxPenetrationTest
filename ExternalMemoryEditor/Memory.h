#pragma once
#include <vector>
#include "RBXStructs.h"

class Memory {
	public:
		static DWORD GetProcessID(const wchar_t* processName);

		static uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

		static DWORD Scan(HANDLE handle, DWORD baseAddress, DWORD VFTableAddress);

		template <typename T>
		static T Read(HANDLE handle, LPCVOID address, SIZE_T size = sizeof(T));

		template <typename T>
		static bool Write(HANDLE handle, LPVOID address, LPCVOID buffer, SIZE_T size = sizeof(T));

		static DWORD GetPointerAddress(HANDLE handle, DWORD address);

		static uintptr_t GetDMAAddress(HANDLE handle, uintptr_t ptr, std::vector<unsigned int> offsets);

		static std::string ReadStringOfUnknownLength(HANDLE handle, DWORD address);

		static void* CreateCharPointerString(HANDLE handle, const char* string);

		static DWORD GetCharacter(HANDLE handle, DWORD player);

		static std::string GetName(HANDLE handle, DWORD instance);

		static std::string GetClassType(HANDLE handle, DWORD instance); // GetClassName.

		static std::vector<DWORD> GetChildren(HANDLE handle, DWORD instance);

		static DWORD GetService(HANDLE handle, DWORD game, std::string className);

		static DWORD FindFirstChild(HANDLE handle, DWORD instance, std::string name);

		static Vector3 GetPosition(HANDLE handle, DWORD instance);

		static void SetPosition(HANDLE handle, DWORD instance, Vector3 position);
};