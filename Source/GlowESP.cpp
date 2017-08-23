#include "GlowESP.h"

GlowColor 	CGreen = { 0.f, 0.4f, 0.f, 1.f};
GlowColor 	CYellow = { 0.4f, 0.4f, 0.f, 1.f};
GlowColor 	CRed = { 0.4f, 0.f, 0.f, 1.f};
GlowColor 	CBlue = { 0.f, 0.f, 0.4f, 1.f};
GlowColor 	COrange = { 1.f, 0.5, 0.f, 1.f};
GlowColor 	CPink = { 1.f, 0.55f, 0.7f, 1.f};
GlowColor 	CWhite = { 1.f, 1.f, 1.f, 1.f};

void CGlowESP::Tick()
{
	auto GlowPointer = Meme.Read<DWORD>(Client + Offsets->m_dwGlowObject);
	auto MaxObjects = Meme.Read<int>(Client + Offsets->m_dwEntityList + 0x4);
	
	if (!GlowPointer)
		return;

	for (int g = 0; g < MaxObjects; g++)
	{			
		auto glowBase = GlowPointer + (g * sizeof(GlowObjectDefinition_t));
		glowObj = Meme.Read<GlowObjectDefinition_t>(glowBase);
		
		if (glowObj.dwBase == NULL)
			continue;
			
		int loopEntityId;
		CEntity* pEntity = glowObj.GetEntityByBase(loopEntityId);
		if (!pEntity)
			continue;
		
		if (!pEntity->bDormant && pEntity->isAlive)
		{
			
			if (VisibleCheck->IsVisible(loopEntityId))
				DrawGlow(glowBase, CGreen);
			else
				DrawGlow(glowBase, CWhite);
		}
	}
}

void CGlowESP::DrawGlow(DWORD dwBase, GlowColor Color)
{
	glowObj.r = Color.r;
	glowObj.g = Color.g;
	glowObj.b = Color.b;
	glowObj.a = Color.a;
	glowObj.m_bRenderWhenOccluded = true;
	glowObj.m_bRenderWhenUnoccluded = false;
	glowObj.m_bFullBloom = false;
	Meme.Write<GlowObjectDefinition_t>(dwBase, glowObj);
}

CEntity* GlowObjectDefinition_t::GetEntityByBase(int& loopId)
{
    for (int i = 0; i < 64; i++)
	{
        if (dwBase == Entity[i].dwBase)
		{
			loopId = i;
            return &Entity[i];
        }
    }
    return nullptr;
}