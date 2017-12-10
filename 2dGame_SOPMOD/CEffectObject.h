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
	CEffectObject();
	CEffectObject(DWORD attackID, DWORD damagedID);
	virtual ~CEffectObject();

	virtual bool Action(void);
	virtual bool Draw(void);

private:
	bool _bEffectStart;
	DWORD _dwAttackID;

	// 이펙트 액션..
	// 일치하는 아이디를 찾고 어떤 공격인지 판별해서
	// 해당 공격의 이펙트 시작지점에서(GetSprite) 이펙트를 그린다. (_bEffectStart를 true로?)

	// 어떤 공격인지는 해당 플레이어 오브젝트의 _dwActionCur 를 보면 되나?

	// 이펙트 드로우..
	// _bEffectStart가 true인걸 그린다.
};
