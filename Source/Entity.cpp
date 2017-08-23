#include "Entity.h"

CEntity LocalPlayer;
CEntity Entity[64];

void CEntity::ReadInfo(DWORD dwBase)
{
	if (!dwBase)
		return;

	Meme.Read(dwBase, chunk, sizeof(chunk));

	this->dwBase	= dwBase;
	iTeam 			= *reinterpret_cast<int*>(chunk + Offsets->m_iTeamNum);
	iIndex 			= *reinterpret_cast<int*>(chunk + Offsets->m_iIndex);
	iHealth 		= *reinterpret_cast<int*>(chunk + Offsets->m_iHealth);
	bDormant 		= *reinterpret_cast<bool*>(chunk + Offsets->m_bDormant);
	isAlive 		= !*reinterpret_cast<bool*>(chunk + Offsets->m_lifeState);
}