#include <iostream>
#include <Windows.h>
#include <vector>
#include "Aimbot.h"
#include "Render.h"
#include "Memory.h"

HWND hwndA;
DWORD playersServiceA;
DWORD localPlayerA;

bool aimbotEnabled = false;
bool teamCheckA = false;

Vector2 SubtractVector2(Vector2 operand1, Vector2 operand2) {
    return {
        operand1.X - operand2.X,
        operand1.Y - operand2.Y
    };
}

float GetVector2Magnitude(Vector2 vector) {
    return sqrtf((vector.X * vector.X) + (vector.Y * vector.Y));
}

Vector2 getClosestCharacterScreenPosToCursor() {
    //DWORD closestCharacter = 0;
    Vector2 screenPos = { -1, -1 };
    int shortestDistance = 9e9;

    DWORD localTeam = 0;
    if (teamCheckA) {
        localTeam = Memory::GetTeam(localPlayerA);
    }

	std::vector<DWORD> players = Memory::GetPlayers(playersServiceA);
	for (DWORD player : players) {
        if (player == localPlayerA) continue;

        DWORD team = Memory::GetTeam(player);
        if (teamCheckA && (team == localTeam)) continue;

		DWORD character = Memory::GetCharacter(player);
		if (!character) continue;

		DWORD humanoidRootPart = Memory::FindFirstChild(character, "HumanoidRootPart");
        //DWORD humanoid = Memory::FindFirstChild(handle, character, "Humanoid");

        if (!humanoidRootPart/* || !humanoid*/) continue;
        //if (Memory::GetHealth(handle, humanoid) == 0) continue;
        
        Vector2 characterScreenPos = Render::WorldToScreenPoint(Memory::GetPosition(humanoidRootPart));
        if (characterScreenPos.X == -1) continue;

        Vector2 cursorPos;
        POINT temp;
        GetCursorPos(&temp);
        cursorPos = { (float)temp.x, (float)temp.y };

        float magnitude = GetVector2Magnitude(SubtractVector2(characterScreenPos, cursorPos));
        
        if (magnitude < shortestDistance) {
            //closestCharacter = character;
            screenPos = characterScreenPos;
            shortestDistance = magnitude;
        }
	}

    return screenPos;
}

Vector2 getRelative(Vector2 position) {
    Vector2 newPosition = { 0, 0 };

    Vector2 cursorPosition;
    POINT temp;
    GetCursorPos(&temp);
    ScreenToClient(hwndA, &temp);
    cursorPosition = { (float)temp.x, (float)temp.y };

    newPosition.X = (position.X - cursorPosition.X) / 4;
    newPosition.Y = (position.Y - cursorPosition.Y) / 4;

    return newPosition;
}

Vector2 normalize(Vector2 position, Vector2 windowDimensions) {
    Vector2 normalized;

    normalized.X = position.X * (65536 / windowDimensions.X);
    normalized.Y = position.Y * (65536 / windowDimensions.Y);

    return normalized;
}

void loop() {
	while (aimbotEnabled) {
        if (GetForegroundWindow() != hwndA) continue;

        if (GetAsyncKeyState(VK_RBUTTON)) {
		    Vector2 closestCharacterScreenPos = getClosestCharacterScreenPosToCursor();
            if (closestCharacterScreenPos.X == -1) continue;

            Vector2 relative = getRelative(closestCharacterScreenPos);

            INPUT input;
            //ZeroMemory(&input, sizeof(INPUT));
            input.mi.time = 0; // Alternative to ZeroMemory (stops screensaver from mode)
            input.type = INPUT_MOUSE;
            input.mi.mouseData = 0;
            input.mi.dx = relative.X;
            input.mi.dy = relative.Y;
            input.mi.dwFlags = MOUSEEVENTF_MOVE;
            SendInput(1, &input, sizeof(input));
        }

        Sleep(1);
	}
}

void Aimbot::SetTeamCheck(bool enabled) {
    teamCheckA = enabled;
}

void Aimbot::Start() {
    if (aimbotEnabled) return;
    aimbotEnabled = true;
    loop();
}

void Aimbot::Stop() {
    aimbotEnabled = false;
}

Aimbot::Aimbot(HWND hwnd_, DWORD playersService_, DWORD localPlayer_) {
    hwndA = hwnd_;
	playersServiceA = playersService_;
    localPlayerA = localPlayer_;
}