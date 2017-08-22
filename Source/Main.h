#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include "ProcMem.h"
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

class CEntity
{
public:
	DWORD 	dwBase;
	int 	iTeam;
	int		iIndex;
	int 	iHealth;
	bool	bDormant;
	bool	isAlive;
	
	void ReadInfo(DWORD dwBase);
};

struct GlowObjectDefinition_t
{
	DWORD dwBase;
	float r;
	float g;
	float b;
	float a;
	uint8_t unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	uint8_t unk2[14];
};
	
struct GlowColor
{
	float r;
	float g;
	float b;
	float a;
};

extern ProcMem Meme;
extern DWORD Client;
extern DWORD ClientSize;
extern DWORD Engine;
extern DWORD EngineSize;
extern COffsets* Offsets;