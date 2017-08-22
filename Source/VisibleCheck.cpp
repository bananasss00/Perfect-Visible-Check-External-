#include "VisibleCheck.h"

#pragma optimize( "", off )
bool WINAPI hkCreateMove( float flInputSampleTime , CUserCmd* pCmd )
{
	hkCreateMoveVars_t* Vars = (hkCreateMoveVars_t*)0x00123456; 		// hkCreateMove + 0x9 = pointer to Vars; 
																		// Initialize in InitCreateMoveHook()
	
	DWORD pLocalPlayer 			= *(DWORD*)(Vars->m_dwLocalPlayer);
	UTIL_TraceLine_t TraceLine 	= (UTIL_TraceLine_t)Vars->m_dwUTIL_TraceLine;
	DWORD dwClientState 		= *(DWORD*)(Vars->m_dwClientState);
	
	bool IsInGame 				= *(int*)(dwClientState + 0x108) == 6; 	// 0x108 = m_dwInGame
	if (pCmd->command_number && IsInGame && pLocalPlayer)
	{
		bool localIsAlive = !*(bool*)(pLocalPlayer + Vars->m_lifeState);
		if (localIsAlive) // Check LocalPlayer IsAlive
		{
			// Get Local Player EyePosition 
			Vars->localOrigin = *(Vector*)(pLocalPlayer + Vars->m_vecOrigin);
			Vars->localViewOffset = *(Vector*)(pLocalPlayer + Vars->m_vecViewOffset);
			Vars->localEyePosition.x = Vars->localOrigin.x + Vars->localViewOffset.x;
			Vars->localEyePosition.y = Vars->localOrigin.y + Vars->localViewOffset.y;
			Vars->localEyePosition.z = Vars->localOrigin.z + Vars->localViewOffset.z;
				
			for (int i = 0; i < 64; i++)
			{
				Vars->visibleStruct->player[i] = false;
				DWORD pEntity = *(DWORD*)(Vars->m_dwEntityList + i * 0x10);
				
				if (!pEntity)
					continue;
				if (pEntity == pLocalPlayer)
					continue;
					
				bool isDead 	= *(bool*)(pEntity + Vars->m_lifeState);
				bool isDormant 	= *(bool*)(pEntity + Vars->m_bDormant);
				int  health 	= *(int*)(pEntity + Vars->m_iHealth);
				
				if (isDead || isDormant || health <= 0)
					continue;
				
				// Get Enemy Head Position
				DWORD boneMatrix = *(DWORD*)(pEntity + Vars->m_dwBoneMatrix);
				if (!boneMatrix)
					continue;

				Vars->enemyHeadPosition.x = *(float*)(boneMatrix + 0x30 * 8 + 0x0C);
				Vars->enemyHeadPosition.y = *(float*)(boneMatrix + 0x30 * 8 + 0x1C);
				Vars->enemyHeadPosition.z = *(float*)(boneMatrix + 0x30 * 8 + 0x2C);
				
				// Visible check
				TraceLine(Vars->localEyePosition, Vars->enemyHeadPosition, 0x4600400B, pLocalPlayer, 0, Vars->m_dwTraceOutput);
				
				float fraction = *(float*)(Vars->m_dwTraceOutput + 0x2C); // 0x2C - fraction offset in trace_t struct
				if (fraction > Vars->min_fraction)
				{
					Vars->visibleStruct->player[i] = true;
				}
			}
		}
	}

	return true;
}
DWORD WINAPI hkCreateMoveEnd(){return 0;}
#pragma optimize( "", on )

bool CVisibleCheck::UpdateVisibleStruct()
{
	if (!hooked)
		return false;
	if (!dwVisibleStruct)
		return false;
		
	visibleStruct = Meme.Read<visibleStruct_t>(dwVisibleStruct);
	return true;
}
bool CVisibleCheck::IsVisible(int entityId)
{
	if (!hooked)
		return false;
	if (!dwVisibleStruct)
		return false;
	
	return visibleStruct.player[entityId];
}
bool CVisibleCheck::InitCreateMoveHook()
{
	if (hooked)
		return true;
		
	DWORD m_dwIClientMode, m_dwUTIL_TraceLine;
	
	// Find offsets
	m_dwIClientMode = Meme.SigScan(Client, ClientSize, "\x8B\x0D\x00\x00\x00\x00\xFF\x75\x08\x8B\x01\xFF\x50\x64", "xx????xxxxxxxx") + 0x2;
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	
	m_dwUTIL_TraceLine = Meme.SigScan(Client, ClientSize, "\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x7C\x56\x52", "xxxxxxxxxxx");
	
	printf("m_dwIClientMode %X\n", m_dwIClientMode);
	printf("m_dwUTIL_TraceLine %X\n", m_dwUTIL_TraceLine);
	
	if (!m_dwIClientMode || !m_dwUTIL_TraceLine || !Offsets->m_dwLocalPlayer || !Offsets->m_dwEntityList)
		return false;
	
	// Set CreateMove local Vars
	hkCreateMoveVars_t hkCreateMoveVars;
	hkCreateMoveVars.m_dwUTIL_TraceLine 	= m_dwUTIL_TraceLine;
	hkCreateMoveVars.m_dwClientState 		= Engine + Offsets->m_dwClientState;
	hkCreateMoveVars.m_dwEntityList 		= Client + Offsets->m_dwEntityList;
	hkCreateMoveVars.m_dwLocalPlayer 		= Client + Offsets->m_dwLocalPlayer;
	hkCreateMoveVars.m_dwBoneMatrix 		= Offsets->m_dwBoneMatrix;
	hkCreateMoveVars.m_vecViewOffset 		= Offsets->m_vecViewOffset;
	hkCreateMoveVars.m_vecOrigin 			= Offsets->m_vecOrigin;
	hkCreateMoveVars.m_lifeState 			= Offsets->m_lifeState;
	hkCreateMoveVars.m_iTeamNum 			= Offsets->m_iTeamNum;
	hkCreateMoveVars.m_iHealth 				= Offsets->m_iHealth;
	hkCreateMoveVars.m_bDormant 			= Offsets->m_bDormant;
	hkCreateMoveVars.min_fraction			= 0.97f;
	
	hkCreateMoveVars.m_dwTraceOutput 	= (DWORD)VirtualAllocEx(Meme.hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	hkCreateMoveVars.visibleStruct 		= (visibleStruct_t*)VirtualAllocEx(Meme.hProcess, NULL, sizeof(visibleStruct_t), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	LPVOID shellCodeAddress 			= VirtualAllocEx(Meme.hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	
	printf("hkCreateMoveVars.m_dwTraceOutput %X\n", hkCreateMoveVars.m_dwTraceOutput);
	printf("hkCreateMoveVars.visibleStruct %X\n", (DWORD)hkCreateMoveVars.visibleStruct);
	
	if (!hkCreateMoveVars.m_dwTraceOutput || !hkCreateMoveVars.visibleStruct || !(DWORD)shellCodeAddress)
		return false;
	
	if (!WriteProcessMemory(Meme.hProcess, shellCodeAddress, hkCreateMove, (DWORD)hkCreateMoveEnd - (DWORD)hkCreateMove, NULL))
		return false;
	
	DWORD dw_hkCreateMoveVars = (DWORD)shellCodeAddress + (DWORD)hkCreateMoveEnd - (DWORD)hkCreateMove;
	if (!WriteProcessMemory(Meme.hProcess, (LPVOID)dw_hkCreateMoveVars, &hkCreateMoveVars, sizeof(hkCreateMoveVars_t), NULL))
		return false;
	
	// Set pointer to hkCreateMoveVars
	if (!WriteProcessMemory(Meme.hProcess, (LPVOID)((DWORD)shellCodeAddress + 0x9), &dw_hkCreateMoveVars, sizeof(DWORD), NULL))
		return false;
	
	printf("hkCreateMoveAdress %X\n", (DWORD)shellCodeAddress);
	system("pause");
	
	Hook(m_dwIClientMode, 24, (DWORD)shellCodeAddress);
	
	dwVisibleStruct = (DWORD)hkCreateMoveVars.visibleStruct;
	hooked = true;
	return hooked;
}

DWORD CVisibleCheck::GetVFunc(DWORD inst, int Index)
{
	DWORD table = Meme.Read<DWORD>(inst);
	DWORD func = table + sizeof(DWORD) * Index;
	return func;
}

void CVisibleCheck::Hook(DWORD Instance, int Index, DWORD HookFunc)
{
	uintptr_t VFunc = GetVFunc(Instance, Index);
	Meme.Protection<DWORD>(VFunc);
	Meme.Write<DWORD>(VFunc, HookFunc);
	Meme.Protection<DWORD>(VFunc);
}