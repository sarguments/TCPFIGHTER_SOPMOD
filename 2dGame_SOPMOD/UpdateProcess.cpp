#include "stdafx.h"
#include "hoxy_Header.h"

#include "UpdateProcess.h"

#include "CScreenDib.h"
#include "CSpriteDib.h"

#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"

void KeyProcess(void)
{
	// �� üũ
	if (g_pPlayerObject == nullptr)
	{
		return;
	}

	// �⺻ �Է��� ���ĵ�
	DWORD dwAction = dfACTION_STAND;

	// ����Ű
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dwAction = dfACTION_MOVE_LU;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dwAction = dfACTION_MOVE_RU;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dwAction = dfACTION_MOVE_RD;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000 && GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dwAction = dfACTION_MOVE_LD;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dwAction = dfACTION_MOVE_LL;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dwAction = dfACTION_MOVE_UU;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		dwAction = dfACTION_MOVE_RR;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dwAction = dfACTION_MOVE_DD;
	}

	// �׼�Ű
	if (GetAsyncKeyState(0x5A) & 0x8000) // Z
	{
		dwAction = dfACTION_ATTACK1;
	}
	else if (GetAsyncKeyState(0x58) & 0x8000) // X
	{
		dwAction = dfACTION_ATTACK2;
	}
	else if (GetAsyncKeyState(0x43) & 0x8000) // C
	{
		dwAction = dfACTION_ATTACK3;
	}

	// �÷��̾� ��ü���� �Էµ� ������ �˷��ش�.
	g_pPlayerObject->ActionInput(dwAction);
}

void Action(void)
{
	g_ObjectList.sort([](CBaseObject* a, CBaseObject* b)
	{	// true �� ������, false�� �ڷ�
		// ����Ʈ�� �� ������
		if (a->GetObjectType() == e_OBJECT_TYPE::eTYPE_EFFECT)
		{
			return false;
		}

		if (b->GetObjectType() == e_OBJECT_TYPE::eTYPE_EFFECT)
		{
			return true;
		}

		// �����ִ� ĳ���Ͱ� �ָ��ִ� ���̹Ƿ� ���� ��´�.
		// ���� �������� ����
		return a->GetCurY() < b->GetCurY();
	});

	std::list<CBaseObject*>::iterator nowIter = g_ObjectList.begin();
	std::list<CBaseObject*>::iterator endIter = g_ObjectList.end();

	// List ���鼭 Action ȣ��
	while (nowIter != endIter)
	{
		if ((*nowIter)->Action() == true)
		{
			delete (*nowIter);
			nowIter = g_ObjectList.erase(nowIter);

			wcout << L"Object Removed" << endl;

			continue;
		}

		nowIter++;
	}
}

void Draw(void)
{
	// ��� ���� ��ü �������� ���� ���÷� ������ �޾Ƽ� ����.
	CScreenDib* pLocalScDib = &g_ScreenDib;
	CSpriteDib* pLocalSpDib = &g_SpriteDib;

	// ���� �̹��� ������, �ʺ�, ����, ��ġ - �ʼ� 4���� - ���߿� ��ο쿡 �׻� �� ���� ����
	// ��� ���� ������ �� ���� ����
	BYTE* bypDest = pLocalScDib->GetDibBuffer();
	int iDestWidth = pLocalScDib->GetWidth();
	int iDestHeight = pLocalScDib->GetHeight();
	int iDestPitch = pLocalScDib->GetPitch();

	//------------------------------------------------------------
	// 1. �� ȭ�� ���
	//------------------------------------------------------------
	pLocalSpDib->DrawImage(0, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

	//------------------------------------------------------------
	// 2. ĳ���� ��� ( ��Ÿ ������Ʈ )
	//------------------------------------------------------------
	for (auto& i : g_ObjectList)
	{
		i->Draw();
	}

	// Dib ������ ������ ȭ������ ���
	pLocalScDib->DrawBuffer(g_hWnd);

	// ������� ����
	Sleep(0);
}