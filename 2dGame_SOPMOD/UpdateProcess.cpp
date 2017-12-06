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
	// 널 체크
	if (g_pPlayerObject == nullptr)
	{
		return;
	}

	// 기본 입력은 스탠드
	DWORD dwAction = dfACTION_STAND;

	// 방향키
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

	// 액션키
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

	// 플레이어 객체에게 입력된 동작을 알려준다.
	g_pPlayerObject->ActionInput(dwAction);
}

void Action(void)
{
	g_ObjectList.sort([](CBaseObject* a, CBaseObject* b)
	{	// true 가 앞으로, false가 뒤로
		// 이펙트는 맨 앞으로
		if (a->GetObjectType() == e_OBJECT_TYPE::eTYPE_EFFECT)
		{
			return false;
		}

		if (b->GetObjectType() == e_OBJECT_TYPE::eTYPE_EFFECT)
		{
			return true;
		}

		// 위에있는 캐릭터가 멀리있는 것이므로 먼저 찍는다.
		// 따라서 오름차순 정렬
		return a->GetCurY() < b->GetCurY();
	});

	std::list<CBaseObject*>::iterator nowIter = g_ObjectList.begin();
	std::list<CBaseObject*>::iterator endIter = g_ObjectList.end();

	// List 돌면서 Action 호출
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
	// 계속 전역 객체 참조하지 말고 로컬로 포인터 받아서 쓴다.
	CScreenDib* pLocalScDib = &g_ScreenDib;
	CSpriteDib* pLocalSpDib = &g_SpriteDib;

	// 실제 이미지 시작점, 너비, 높이, 피치 - 필수 4가지 - 나중에 드로우에 항상 이 정보 전달
	// 출력 버퍼 포인터 및 정보 얻음
	BYTE* bypDest = pLocalScDib->GetDibBuffer();
	int iDestWidth = pLocalScDib->GetWidth();
	int iDestHeight = pLocalScDib->GetHeight();
	int iDestPitch = pLocalScDib->GetPitch();

	//------------------------------------------------------------
	// 1. 맵 화면 출력
	//------------------------------------------------------------
	pLocalSpDib->DrawImage(0, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

	//------------------------------------------------------------
	// 2. 캐릭터 출력 ( 기타 오브젝트 )
	//------------------------------------------------------------
	for (auto& i : g_ObjectList)
	{
		i->Draw();
	}

	// Dib 버퍼의 내용을 화면으로 출력
	pLocalScDib->DrawBuffer(g_hWnd);

	// 기아현상 방지
	Sleep(0);
}