#pragma once
#include "Main.h"

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
private:
	BYTE chunk[0x300];
};

extern CEntity LocalPlayer;
extern CEntity Entity[64];