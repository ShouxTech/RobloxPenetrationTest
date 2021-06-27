#pragma once
#include <Windows.h>

class LuaStateHook {
	public:
		static DWORD Hook(HANDLE handle , DWORD baseAddress);
};