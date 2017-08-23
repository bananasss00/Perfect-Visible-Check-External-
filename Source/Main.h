#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>

#include "ProcMem.h"
#include "Entity.h"
#include "GlowESP.h"
#include "VisibleCheck.h"

using namespace std;

class COffsets
{
public:
	DWORD m_dwClientState = 0x5A4334;
	DWORD m_dwLocalPlayer = 0xAA8BBC;
	DWORD m_dwEntityList = 0x4A85AAC;
	DWORD m_dwGlowObject = 0x4FA2238;
	DWORD m_dwBoneMatrix = 0x2698;
	DWORD m_iTeamNum = 0xF0;
	DWORD m_iHealth = 0xFC;
	DWORD m_vecViewOffset = 0x104;
	DWORD m_vecOrigin = 0x134;
	DWORD m_lifeState = 0x25B;
	DWORD m_bDormant = 0xE9;
	DWORD m_iIndex = 0x64;
};
extern COffsets* Offsets;

extern ProcMem Meme;
extern DWORD Client;
extern DWORD ClientSize;
extern DWORD Engine;
extern DWORD EngineSize;