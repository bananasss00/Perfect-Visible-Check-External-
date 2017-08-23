#include "Main.h"
COffsets* 		Offsets = new COffsets();
CGlowESP* 		GlowESP = new CGlowESP();
CVisibleCheck* 	VisibleCheck;

ProcMem Meme;
DWORD Client; DWORD ClientSize;
DWORD Engine; DWORD EngineSize;

void InitializeVisibleCheck()
{
	VisibleCheck = new CVisibleCheck(Meme.hProcess);
	
	// Find offsets
	DWORD m_dwIClientMode, m_dwUTIL_TraceLine, m_nDeltaTick;
	
	m_dwIClientMode = Meme.SigScan(Client, ClientSize, "\x8B\x0D\x00\x00\x00\x00\xFF\x75\x08\x8B\x01\xFF\x50\x64", "xx????xxxxxxxx") + 0x2;
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	
	m_dwUTIL_TraceLine = Meme.SigScan(Client, ClientSize, "\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x7C\x56\x52", "xxxxxxxxxxx");
	
	m_nDeltaTick = Meme.SigScan(Engine, EngineSize, "\x83\xBE\x00\x00\x00\x00\xFF\x74\x25\x8D\x4E\x08\xE8", "xx????xxxxxxx") + 0x2;
	m_nDeltaTick = Meme.Read<DWORD>(m_nDeltaTick);
	
	DWORD m_dwClientState = Meme.Read<DWORD>(Engine + Offsets->m_dwClientState);
	DWORD m_dwLocalPlayer = Client + Offsets->m_dwLocalPlayer;
	DWORD m_dwEntityList = Client + Offsets->m_dwEntityList;
	
	// Set hook
	VisibleCheck->InitCreateMoveHook(
			m_dwIClientMode, 
			m_dwUTIL_TraceLine,
			m_dwLocalPlayer,
			m_dwEntityList,
			m_dwClientState,
			m_nDeltaTick,
			Offsets->m_dwBoneMatrix,
			Offsets->m_vecViewOffset,
			Offsets->m_vecOrigin,
			Offsets->m_lifeState,
			Offsets->m_iTeamNum,
			Offsets->m_iHealth,
			Offsets->m_bDormant);
}

int main()
{
	Meme.Process("csgo.exe");
	Client = Meme.Module("client.dll"); ClientSize = Meme.ModuleSize("client.dll");
	Engine = Meme.Module("engine.dll"); EngineSize = Meme.ModuleSize("engine.dll");
	
	SetConsoleTitle("pVisibleCheck");
	cout << "Found csgo.exe" << endl;
	
	InitializeVisibleCheck();
	
	system("cls");
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 10);
	cout << "       [GlowESP]" << endl;
	cout << "========================" << endl;
	cout << "visible     = green" << endl;
	cout << "not_visible = white" << endl;
	cout << "========================" << endl;
	
	// Main Loop //
	for (;;Sleep(1))
	{
		DWORD LocalBase = Meme.Read<DWORD>(Client + Offsets->m_dwLocalPlayer);
		LocalPlayer.ReadInfo(LocalBase);
		
		for (int i = 0; i < 64; i++)
		{		
			DWORD EntityBase = Meme.Read<DWORD>((Client + Offsets->m_dwEntityList) + i * 0x10);
			if (!EntityBase)
				continue;
		
			Entity[i].ReadInfo(EntityBase);
		}
		
		VisibleCheck->UpdateVisibleStruct();
		GlowESP->Tick();
	}

	return(0);
}