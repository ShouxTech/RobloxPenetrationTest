#pragma once
#include "RBXStructs.h"

class Render {
	private:
		//HANDLE Handle;
		//DWORD VisualEngine;
		//DWORD ViewMatrixAddress;
	public:
		static Vector2 GetWindowDimensions();

		static Vector2 WorldToScreenPoint(Vector3 position);

		void SetTeamCheck(bool enabled);
		void SetCrosshair(bool enabled);

		void EnableESP();
		void DisableESP();

		Render(HANDLE handle, DWORD playersService_, DWORD localPlayer_, DWORD visualEngine_);
};

