#include "stdafx.h"
#include "hoxy_Header.h"

#include "CEffectObject.h"
#include "CPlayerObject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"

CEffectObject::CEffectObject(DWORD attackID, DWORD damagedID)
	: _bEffectStart(false)
{
	_dwAttackID = attackID;

	// ��ġ�ϴ� ���̵� ã��
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == damagedID)
		{
			return true;
		}
		return false;
	});

	CBaseObject* pPlayerObj = (*iter);
	int plyaerX = pPlayerObj->GetCurX();
	int playerY = pPlayerObj->GetCurY();
	SetPosition(plyaerX, playerY - 40);

	SetObjectType(e_OBJECT_TYPE::eTYPE_EFFECT);
	SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_04, dfDELAY_EFFECT);

	static int effectNum = 100;
	SetObjectID(effectNum);
	++effectNum;
}

CEffectObject::~CEffectObject()
{
}

bool CEffectObject::Action(void)
{
	// ���� ����������
	if (_bEffectStart)
	{
		NextFrame();
	}

	// ��ġ�ϴ� ���̵� ã��
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == _dwAttackID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		// TODO : �ϴ� ��ã���� ����
		wcout << L"Effect Find FAIL" << endl;
		return true;
	}
	//wcout << L"Effect Find.. ATTACK id: " << _dwAttackID << L" // Effect ID: " << GetObjectID() << endl;

	// � �������� �Ǻ��ؼ�
	int curAction = ((CPlayerObject*)(*iter))->GetActionCur();
	int nowSprite = ((CPlayerObject*)(*iter))->GetSprite();

	switch (curAction)
	{
	case dfACTION_ATTACK1:
	{
		wcout << L"dfACTION_ATTACK1" << endl;

		// �ش� ������ ����Ʈ ������������(GetSprite) ����Ʈ�� �׸���.
		if (nowSprite == ePLAYER_ATTACK1_L02 || nowSprite == ePLAYER_ATTACK1_R02)
		{
			wcout << L"_bEffectStart = true" << endl;
			_bEffectStart = true;
		}
	}
	break;
	case dfACTION_ATTACK2:
	{
		wcout << L"dfACTION_ATTACK2" << endl;

		if (nowSprite == ePLAYER_ATTACK2_L02 || nowSprite == ePLAYER_ATTACK2_R02)
		{
			wcout << L"_bEffectStart = true" << endl;
			_bEffectStart = true;
		}
	}
	break;
	case dfACTION_ATTACK3:
	{
		wcout << L"dfACTION_ATTACK3" << endl;

		if (nowSprite == ePLAYER_ATTACK3_L03 || nowSprite == ePLAYER_ATTACK3_R03)
		{
			wcout << L"_bEffectStart = true" << endl;
			_bEffectStart = true;
		}
	}
	break;
	}

	// �������̰� ���� ���������� ����
	if (isEndFrame() == true)
	{
		return true;
	}

	return false;
}

bool CEffectObject::Draw(void)
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

	// _bEffectStart�� true�ΰ� �׸���.
	if (_bEffectStart == false)
	{
		return false;
	}

	// ����Ʈ �׸���
	pLocalSpDib->DrawSprite(GetSprite(),
		_iCurX, _iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);

	return true;
}