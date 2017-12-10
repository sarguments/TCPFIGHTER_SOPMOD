#pragma once
#include "CBaseObject.h"

extern class CScreenDib g_ScreenDib;
extern class CSpriteDib g_SpriteDib;

extern HWND g_hWnd;
extern std::list<CBaseObject*> g_ObjectList;

// ����Ʈ�� ���� ������ ��� ���� �ִ´�.

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

	// ����Ʈ �׼�..
	// ��ġ�ϴ� ���̵� ã�� � �������� �Ǻ��ؼ�
	// �ش� ������ ����Ʈ ������������(GetSprite) ����Ʈ�� �׸���. (_bEffectStart�� true��?)

	// � ���������� �ش� �÷��̾� ������Ʈ�� _dwActionCur �� ���� �ǳ�?

	// ����Ʈ ��ο�..
	// _bEffectStart�� true�ΰ� �׸���.
};
