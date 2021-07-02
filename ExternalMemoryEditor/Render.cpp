#include <iostream>
#include <Windows.h>
#include <vector>
#include "Render.h"
#include "Memory.h"
#include "DirectOverlay.h"

HANDLE handleR;
DWORD playersServiceR;
DWORD localPlayerR;
DWORD visualEngineR;

bool espEnabled = false;
bool crosshairEnabled = false;
bool teamCheckR = false;

Vector3 AddVector3(Vector3 operand1, Vector3 operand2) {
	return {
		operand1.X + operand2.X,
		operand1.Y + operand2.Y,
		operand1.Z + operand2.Z
	};
}

Vector3 SubtractVector3(Vector3 operand1, Vector3 operand2) {
	return {
		operand1.X - operand2.X,
		operand1.Y - operand2.Y,
		operand1.Z - operand2.Z
	};
}

float GetVector3Magnitude(Vector3 vector) {
	return sqrtf((vector.X * vector.X) + (vector.Y * vector.Y) + (vector.Z * vector.Z));
}

void drawLoop(int width, int height) {
	//DrawLine(0, 0, 100, 100, 5, 1, 1, 0, 0.8);
	//DrawBox(100, 100, 100, 100, 5, 0, 1, 0, 1, 0);
	//DrawCircle(50, 50, 20, 1, 1, 0, 0, .25, 1);

	if (crosshairEnabled) {
		Vector2 windowDimensions = Render::GetWindowDimensions();
		Vector2 screenCenter = { windowDimensions.X / 2, windowDimensions.Y / 2 };

		DrawLine(
			screenCenter.X - 20, screenCenter.Y,
			screenCenter.X + 20, screenCenter.Y,
			2,
			1, 0, 0, 1
		);

		DrawLine(
			screenCenter.X, screenCenter.Y - 20,
			screenCenter.X, screenCenter.Y + 20,
			2,
			1, 0, 0, 1
		);
	}

	if (!espEnabled) return;

	DWORD localTeam = 0;
	if (teamCheckR) {
		localTeam = Memory::GetTeam(localPlayerR);
	}

	/*DWORD localCharacter = Memory::GetCharacter(localPlayerR);
	if (!localCharacter) return;

	DWORD localHumanoidRootPart = Memory::FindFirstChild(localCharacter, "HumanoidRootPart");
	if (!localHumanoidRootPart) return;

	Vector3 localHumanoidRootPartPosition = Memory::GetPosition(localHumanoidRootPart);
	if (localHumanoidRootPartPosition.X == -1) return;*/

	Vector3 cameraPosition = Memory::GetCameraPosition(visualEngineR);

	std::vector<DWORD> players = Memory::GetPlayers(playersServiceR);
	for (DWORD player : players) {
		if (player == localPlayerR) continue;

		DWORD team = Memory::GetTeam(player);
		if (teamCheckR && (team == localTeam)) continue;

		DWORD character = Memory::GetCharacter(player);
		if (!character) continue;

		DWORD humanoidRootPart = Memory::FindFirstChild(character, "HumanoidRootPart");
		//DWORD humanoid = Memory::FindFirstChild(handle, character, "Humanoid");

		if (!humanoidRootPart/* || !humanoid*/) continue;
		//if (Memory::GetHealth(handle, humanoid) == 0) continue;

		Vector3 humanoidRootPartPosition = Memory::GetPosition(humanoidRootPart);

		//Vector2 humanoidRootPartScreenPos = Render::WorldToScreenPoint(handleR, visualEngineR, humanoidRootPartPosition);
		//if (humanoidRootPartScreenPos.X == -1) continue;

		Vector2 headScreenPos = Render::WorldToScreenPoint(AddVector3(humanoidRootPartPosition, { 0, 2, 0 }));
		if (headScreenPos.X == -1) continue;

		Vector2 legScreenPos = Render::WorldToScreenPoint(SubtractVector3(humanoidRootPartPosition, { 0, 3, 0 }));
		if (legScreenPos.X == -1) continue;

		//float distanceFromLocalHumanoidRootPartPosition = GetVector3Magnitude(SubtractVector3(humanoidRootPartPosition, localHumanoidRootPartPosition));
		float distanceFromCamera = GetVector3Magnitude(SubtractVector3(cameraPosition, humanoidRootPartPosition));

		float height = legScreenPos.Y - headScreenPos.Y;
		float width = height / 2;

		float fontSize = 13;

		DrawBox(
			headScreenPos.X - (width / 2), headScreenPos.Y,
			width, height,
			3,
			1, 0, 0, 1,
			false
		);
		
		DrawString(
			Memory::GetName(player),
			fontSize,
			headScreenPos.X, headScreenPos.Y - 25,
			1, 1, 1, 1
		);

		DrawString(
			std::to_string((int)distanceFromCamera),
			fontSize,
			headScreenPos.X, legScreenPos.Y + 5,
			1, 1, 1, 1
		);
	}
}

Vector2 Render::GetWindowDimensions() {
	Vector2 dimensions = { 0, 0 };
	
	DWORD offset = 0x4D8;
	int width = Memory::Read<int>((LPCVOID)(visualEngineR + offset));
	int height = Memory::Read<int>((LPCVOID)(visualEngineR + offset + sizeof(width)));

	dimensions.X = width;
	dimensions.Y = height;

	return dimensions;
}

Vector2 Render::WorldToScreenPoint(Vector3 position) {
	Vector2 windowDimensions = Render::GetWindowDimensions();

	float viewMatrix[16];
	ReadProcessMemory(handleR, (LPCVOID)(visualEngineR + 0x120), &viewMatrix, sizeof(viewMatrix), 0);
	
	Vector4 clipCoords;
	clipCoords.X = (position.X * viewMatrix[0]) + (position.Y * viewMatrix[1]) + (position.Z * viewMatrix[2]) + viewMatrix[3];
	clipCoords.Y = (position.X * viewMatrix[4]) + (position.Y * viewMatrix[5]) + (position.Z * viewMatrix[6]) + viewMatrix[7];
	clipCoords.Z = (position.X * viewMatrix[8]) + (position.Y * viewMatrix[9]) + (position.Z * viewMatrix[10]) + viewMatrix[11];
	clipCoords.W = (position.X * viewMatrix[12]) + (position.Y * viewMatrix[13]) + (position.Z * viewMatrix[14]) + viewMatrix[15];

	if (clipCoords.W < 0.1f) {
		return { -1, -1 }; // Off screen.
	}

	Vector3 NDC;
	NDC.X = clipCoords.X / clipCoords.W;
	NDC.Y = clipCoords.Y / clipCoords.W;
	NDC.Z = clipCoords.Z / clipCoords.W;

	return {
		(windowDimensions.X / 2 * NDC.X) + (NDC.X + windowDimensions.X / 2),
		-(windowDimensions.Y / 2 * NDC.Y) + (NDC.Y + windowDimensions.Y / 2)
	};
}

void Render::SetTeamCheck(bool enabled) {
	teamCheckR = enabled;
}

void Render::SetCrosshair(bool enabled) {
	crosshairEnabled = enabled;
}

void Render::EnableESP() {
	espEnabled = true;
}

void Render::DisableESP() {
	espEnabled = false;
}

Render::Render(HANDLE handle_, DWORD playersService_, DWORD localPlayer_, DWORD visualEngine_) {
	handleR = handle_;
	playersServiceR = playersService_;
	localPlayerR = localPlayer_;
	visualEngineR = visualEngine_;
	DirectOverlaySetOption(D2DOV_FONT_COURIER | D2DOV_REQUIRE_FOREGROUND);
	DirectOverlaySetup(drawLoop, FindWindowA(NULL, "Roblox"));
	//this->Handle = handle;
	//this->VisualEngine = visualEngine;
	//this->ViewMatrixAddress = visualEngine + 0x120;
}