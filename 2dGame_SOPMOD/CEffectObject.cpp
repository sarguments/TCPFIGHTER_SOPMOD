#include "stdafx.h"
#include "CEffectObject.h"
#include "CPlayerObject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"

CEffectObject::CEffectObject()
{
	// TODO : ����Ʈ ������
}

CEffectObject::CEffectObject(DWORD dwAttackID)
	: _bEffectStart(false)
{
	_dwAttackID = dwAttackID;
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
	NextFrame();

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
		wcout << L"Effect Find FAIL" << endl;
		return false;
	}
	wcout << L"Effect Find : " << (*iter)->GetObjectID() << endl;

	// � �������� �Ǻ��ؼ�
	int curAction = ((CPlayerObject*)(*iter))->GetActionCur();
	int nowSprite = ((CPlayerObject*)(*iter))->GetSprite();
	//int playerDir = ((CPlayerObject*)(*iter))->GetDirection();

	switch (curAction)
	{
	case dfACTION_ATTACK1:
	{
		// �ش� ������ ����Ʈ ������������(GetSprite) ����Ʈ�� �׸���.
		if (nowSprite == ePLAYER_ATTACK1_L02 || nowSprite == ePLAYER_ATTACK1_R02)
		{
			_bEffectStart = true;
		}
	}
	break;
	case dfACTION_ATTACK2:
	{
		if (nowSprite == ePLAYER_ATTACK2_L02 || nowSprite == ePLAYER_ATTACK2_R02)
		{
			_bEffectStart = true;
		}
	}
	break;
	case dfACTION_ATTACK3:
	{
		if (nowSprite == ePLAYER_ATTACK3_L03 || nowSprite == ePLAYER_ATTACK3_R03)
		{
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
	// TODO : ����Ʈ �׸��� �ӽ�

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

	pLocalSpDib->DrawSprite(GetSprite(),
		100 + 20, 100 + 20, bypDest, iDestWidth, iDestHeight, iDestPitch);

	return true;
}