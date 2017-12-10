#pragma once
#include "CBaseObject.h"

extern class CScreenDib g_ScreenDib;
extern class CSpriteDib g_SpriteDib;

extern HWND g_hWnd;
extern std::list<CBaseObject*> g_ObjectList;

// 이펙트가 언제 터질지 계속 보고 있는다.

class CEffectObject :
	public CBaseObject
{
public:
	CEffectObject(DWORD attackID, DWORD damagedID);
	virtual ~CEffectObject();

	virtual bool Action(void);
	virtual bool Draw(void);

private:
	bool _bEffectStart;
	DWORD _dwAttackID;
};
