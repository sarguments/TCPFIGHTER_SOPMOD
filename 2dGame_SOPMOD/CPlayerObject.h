#pragma once
#include "CBaseObject.h"

extern class CScreenDib g_ScreenDib;
extern class CSpriteDib g_SpriteDib;

extern HWND g_hWnd;
extern std::list<CBaseObject*> g_ObjectList;

class CPlayerObject :
	public CBaseObject
{
public:
	//CPlayerObject(CHAR hp);
	CPlayerObject(CHAR hp, bool bPlayer, int dir);
	virtual ~CPlayerObject();

	virtual bool Action(void);
	virtual bool Draw(void);

	void ActionProc(void);
	int GetDirection(void);
	CHAR GetHP(void);
	bool isPlayer(void);
	void SetDirection(int param);
	void SetHP(CHAR param);

	DWORD GetActionCur(void);

private:
	bool InputActionProc();
	void SetActionAttack1(void);
	void SetActionAttack2(void);
	void SetActionAttack3(void);
	void SetActionMove(DWORD action);
	void SetActionStand(void);

public:
	DWORD _dwActionCur;
	DWORD _dwActionOld;
	bool _bPlayerCharacter;
	bool _isAttackPossible = false;

private:
	// TODO : ����� private
	//bool _bPlayerCharacter;
	CHAR _chHP;
	//DWORD _dwActionCur;
	//DWORD _dwActionOld;
	int _iDirCur;
	int _iDirOld;
};
