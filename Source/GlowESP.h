#pragma once
#include "Main.h"

class CEntity;

struct GlowObjectDefinition_t
{
	CEntity* GetEntityByBase(int& loopId);

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

class CGlowESP
{
public:
	void Tick();
private:
	void DrawGlow(DWORD dwBase, GlowColor Color);

	GlowObjectDefinition_t 	glowObj;
};

extern CGlowESP* GlowESP;