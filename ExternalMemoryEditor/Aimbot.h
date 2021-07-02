#pragma once

class Aimbot {
	public:
		void SetTeamCheck(bool enabled);

		void Start();
		void Stop();

		Aimbot(HWND hwnd_, DWORD playersService_, DWORD localPlayer_);
};

