#pragma once
#include <Windows.h>

struct Addresses {
	// Contains offset of address from base address.

	// VFTables.
	static const DWORD DataModel = 0x1CACFD8;

	// Other.
	static const DWORD PlaceID = 0x24F2A40;
	static const DWORD Print = 0x23a190;
	static const DWORD Index2Adr = 0x11E5A53;
};