#include "Main.h"
COffsets* Offsets = new COffsets();
CVisibleCheck* VisibleCheck;

ProcMem Meme;

DWORD Client; DWORD ClientSize;
DWORD Engine; DWORD EngineSize;

CEntity LocalPlayer;
CEntity Entity[64];

/* PRE-CALL FUNCTION'S */
void 	InitializeVisibleCheck();
DWORD 	WINAPI data(LPVOID lpParam);
DWORD 	WINAPI ESP(LPVOID lpParam);

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
	
	HANDLE rghThreads[] = {
		CreateThread(NULL, NULL, data, NULL, NULL, NULL),
		CreateThread(NULL, NULL, ESP, NULL, NULL, NULL),
	};

	WaitForMultipleObjects(sizeof(rghThreads) / sizeof(HANDLE), rghThreads, TRUE, INFINITE);
	return(0);
}

void InitializeVisibleCheck()
{
	VisibleCheck = new CVisibleCheck(Meme.hProcess);
	
	// Find offsets
	DWORD m_dwIClientMode, m_dwUTIL_TraceLine;
	
	m_dwIClientMode = Meme.SigScan(Client, ClientSize, "\x8B\x0D\x00\x00\x00\x00\xFF\x75\x08\x8B\x01\xFF\x50\x64", "xx????xxxxxxxx") + 0x2;
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	m_dwIClientMode = Meme.Read<DWORD>(m_dwIClientMode);
	
	m_dwUTIL_TraceLine = Meme.SigScan(Client, ClientSize, "\x55\x8B\xEC\x83\xE4\xF0\x83\xEC\x7C\x56\x52", "xxxxxxxxxxx");
	
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
			Offsets->m_dwBoneMatrix,
			Offsets->m_vecViewOffset,
			Offsets->m_vecOrigin,
			Offsets->m_lifeState,
			Offsets->m_iTeamNum,
			Offsets->m_iHealth,
			Offsets->m_bDormant);
}
	
/* --------------- */
/*   LOCAL DATA    */
/* --------------- */
void CEntity::ReadInfo(DWORD dwBase)
{
	this->dwBase	= dwBase;
	iTeam 			= Meme.Read<int>(dwBase + Offsets->m_iTeamNum);
	iIndex 			= Meme.Read<int>(dwBase + Offsets->m_iIndex);
	iHealth 		= Meme.Read<int>(dwBase + Offsets->m_iHealth);
	bDormant 		= Meme.Read<bool>(dwBase + Offsets->m_bDormant);
	isAlive 		= !Meme.Read<bool>(dwBase + Offsets->m_lifeState);
}

/* Get Data - Thread */
DWORD WINAPI data(LPVOID lpParam)
{
	while (true) {
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
		Sleep(1);
	}
	return NULL;
}

/* --------------- */
/*       GLOW      */
/* --------------- */
GlowObjectDefinition_t glowObj;

GlowColor CGreen = { 0.f, 0.4f, 0.f, 1.f};
GlowColor CYellow = { 0.4f, 0.4f, 0.f, 1.f};
GlowColor CRed = { 0.4f, 0.f, 0.f, 1.f};
GlowColor CBlue = { 0.f, 0.f, 0.4f, 1.f};
GlowColor COrange = { 1.f, 0.5, 0.f, 1.f};
GlowColor CPink = { 1.f, 0.55f, 0.7f, 1.f};
GlowColor CWhite = { 1.f, 1.f, 1.f, 1.f};

void DrawGlow(DWORD dwBase, GlowColor Color)
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

CEntity* GetEntityByBase(DWORD dwBase, int& loopId)
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

/* GlowESP - Thread */
DWORD WINAPI ESP(LPVOID lpParam)
{	
	while (true) {
		auto GlowPointer = Meme.Read<DWORD>(Client + Offsets->m_dwGlowObject);
		auto MaxObjects = Meme.Read<int>(Client + Offsets->m_dwEntityList + 0x4);
		
		for (int g = 0; g < MaxObjects; g++) /* LOOP START */
		{
			if (GlowPointer == NULL){Sleep(100);break;}
			
			auto glowBase = GlowPointer + (g * sizeof(GlowObjectDefinition_t));
			glowObj = Meme.Read<GlowObjectDefinition_t>(glowBase);
			
			if (glowObj.dwBase == NULL)
				continue;
				
			int loopEntityId = 0;
			CEntity* pEntity = GetEntityByBase(glowObj.dwBase, loopEntityId);
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
		Sleep(1);
	}
	return NULL;
}