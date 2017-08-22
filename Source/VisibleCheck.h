#pragma once
#include "Main.h"

struct visibleStruct_t
{
	bool player[64];
};

class CVisibleCheck
{
public:
	CVisibleCheck() { hooked = false; }
	bool InitCreateMoveHook();
	bool UpdateVisibleStruct();
	bool IsVisible(int entityId);
private:
	bool hooked;
	DWORD dwVisibleStruct;
	visibleStruct_t visibleStruct;
	
	DWORD GetVFunc(DWORD inst, int Index);
	void Hook(DWORD Instance, int Index, DWORD HookFunc);
};

extern CVisibleCheck* VisibleCheck;

struct Vector
{
	float x, y, z;
};

typedef int(__fastcall* UTIL_TraceLine_t)(const Vector&, const Vector&, unsigned int, const ULONG_PTR, int, ULONG_PTR);

struct hkCreateMoveVars_t
{
// Offsets
	DWORD m_dwUTIL_TraceLine;
	DWORD m_dwClientState;
	DWORD m_dwEntityList;
	DWORD m_dwLocalPlayer;
	DWORD m_dwBoneMatrix;
	DWORD m_vecViewOffset;
	DWORD m_vecOrigin;
	DWORD m_lifeState;
	DWORD m_iTeamNum;
	DWORD m_iHealth;
	DWORD m_bDormant;
	DWORD m_dwTraceOutput; // Output trace_t struct
	visibleStruct_t* visibleStruct; // Player's visible struct
// Local variables
	Vector localOrigin;
	Vector localViewOffset;
	Vector localEyePosition;
	Vector enemyHeadPosition;
	float min_fraction;
};

class CUserCmd
{
public:
	virtual ~CUserCmd( ) { };
	int		command_number;
	int		tick_count;
	Vector	viewangles;
	Vector	aimdirection;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	BYTE    impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;
	bool	hasbeenpredicted;
	char	pad_0x4C[0x18];
};