#include "stdafx.h"
#include "hoxy_Header.h"

#include "CPlayerObject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"

#include "NetworkProcess.h"

//CPlayerObject::CPlayerObject(CHAR hp)
//	: _chHP(hp), _bPlayerCharacter(false)
//{
//	SetDirection(dfDIR_LEFT);
//	SetObjectType(e_OBJECT_TYPE::eTYPE_PLAYER);
//}

CPlayerObject::CPlayerObject(CHAR hp, bool bPlayer, int dir)
	: _chHP(hp), _bPlayerCharacter(bPlayer)
{
	//_iDirOld = dir;
	SetDirection(dir);
	SetObjectType(e_OBJECT_TYPE::eTYPE_PLAYER);
}

CPlayerObject::~CPlayerObject()
{
}

bool CPlayerObject::Action(void)
{
	// ���� ���������� �ִϸ��̼�
	NextFrame();
	ActionProc();

	// true�� ����
	return false;
}

bool CPlayerObject::Draw(void)
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

	// �׸���
	pLocalSpDib->DrawSprite50(e_SPRITE::eSHADOW,
		GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);

	// �÷��̾�
	if (_bPlayerCharacter)
	{
		pLocalSpDib->DrawSpriteRed(GetSprite(),
			GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
	else
	{
		pLocalSpDib->DrawSprite(GetSprite(),
			GetCurX(), GetCurY(), bypDest, iDestWidth, iDestHeight, iDestPitch);
	}

	// HP������
	pLocalSpDib->DrawSprite(e_SPRITE::eGUAGE_HP,
		GetCurX() - 35, GetCurY() + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());

	return true;
}

void CPlayerObject::ActionProc(void)
{
	switch (_dwActionCur)
	{
		// ������ ���� ó��
	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
	{
		if (isEndFrame())
		{
			// �⺻�������� ���ư�
			SetActionStand();
		}
	}
	break;
	default:
	{
		// ����� �Է� ó��
		InputActionProc();
	}
	break;
	}

	if (_dwActionOld == _dwActionCur)
	{
		return;
	}

	wcout << L"Old : " << _dwActionOld << L"// Cur : " << _dwActionCur << endl;

	_dwActionOld = _dwActionCur;

	if (_bPlayerCharacter == false)
	{
		return;
	}

	SendPacketProc(_dwActionCur);
}

int CPlayerObject::GetDirection(void)
{
	return _iDirCur;
}

// HP�� CHAR
CHAR CPlayerObject::GetHP(void)
{
	return _chHP;
}

bool CPlayerObject::isPlayer(void)
{
	return _bPlayerCharacter;
}

void CPlayerObject::SetDirection(int param)
{
	_iDirOld = _iDirCur;
	_iDirCur = param;
}

void CPlayerObject::SetHP(CHAR param)
{
	_chHP = param;
}

DWORD CPlayerObject::GetActionCur(void)
{
	return _dwActionCur;
}

// �Է� ó��
void CPlayerObject::InputActionProc()
{
	switch (_dwActionInput)
	{
	case dfACTION_MOVE_LL:
	{
		SetPosition(_iCurX - dfSPEED_PLAYER_X, _iCurY);
		SetDirection(dfDIR_LEFT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_LU:
	{
		SetPosition(_iCurX - dfSPEED_PLAYER_X, _iCurY - dfSPEED_PLAYER_Y);
		SetDirection(dfDIR_LEFT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_UU:
	{
		SetPosition(_iCurX, _iCurY - dfSPEED_PLAYER_Y);
		SetDirection(_iDirCur);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_RU:
	{
		SetPosition(_iCurX + dfSPEED_PLAYER_X, _iCurY - dfSPEED_PLAYER_Y);
		SetDirection(dfDIR_RIGHT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_RR:
	{
		SetPosition(_iCurX + dfSPEED_PLAYER_X, _iCurY);
		SetDirection(dfDIR_RIGHT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_RD:
	{
		SetPosition(_iCurX + dfSPEED_PLAYER_X, _iCurY + dfSPEED_PLAYER_Y);
		SetDirection(dfDIR_RIGHT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_DD:
	{
		SetPosition(_iCurX, _iCurY + dfSPEED_PLAYER_Y);
		SetDirection(_iDirCur);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_MOVE_LD:
	{
		SetPosition(_iCurX - dfSPEED_PLAYER_X, _iCurY + dfSPEED_PLAYER_Y);
		SetDirection(dfDIR_LEFT);
		SetActionMove(_dwActionInput);
	}
	break;
	case dfACTION_ATTACK1:
	{
		// ���� �׼� �Ŀ� �ϴ� _dwActionInput�� 9999 �� ����
		// ���߿� ������ ���� �� _dwActionInput�� STAND�� �����Ҷ�
		// ������ ���ο� �Է��� �������� STAND�� �������� �ʴ´�.
		SetActionAttack1();
		_dwActionInput = dfACTION_STAND;
	}
	break;
	case dfACTION_ATTACK2:
	{
		SetActionAttack2();
		_dwActionInput = dfACTION_STAND;
	}
	break;
	case dfACTION_ATTACK3:
	{
		SetActionAttack3();
		_dwActionInput = dfACTION_STAND;
	}
	break;
	case dfACTION_STAND:
	{
		// �⺻ �Է��� ���ĵ��̴�.
		SetActionStand();
	}
	break;
	}
}

void CPlayerObject::SetActionAttack1(void)
{
	_dwActionOld = _dwActionCur;
	_dwActionCur = dfACTION_ATTACK1;

	// ���⼳��
	int nowDir = GetDirection();
	if (nowDir == dfDIR_LEFT)
	{
		//SetDirection(dfDIR_LEFT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK1_L01, e_SPRITE::ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
	}
	else
	{
		//SetDirection(dfDIR_RIGHT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK1_R01, e_SPRITE::ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);
	}
}

void CPlayerObject::SetActionAttack2(void)
{
	_dwActionOld = _dwActionCur;
	_dwActionCur = dfACTION_ATTACK2;

	int nowDir = GetDirection();
	if (nowDir == dfDIR_LEFT)
	{
		//SetDirection(dfDIR_LEFT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK2_L01, e_SPRITE::ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
	}
	else
	{
		//SetDirection(dfDIR_RIGHT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK2_R01, e_SPRITE::ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);
	}
}

void CPlayerObject::SetActionAttack3(void)
{
	_dwActionOld = _dwActionCur;
	_dwActionCur = dfACTION_ATTACK3;

	int nowDir = GetDirection();
	if (nowDir == dfDIR_LEFT)
	{
		//SetDirection(dfDIR_LEFT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK3_L01, e_SPRITE::ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
	}
	else
	{
		//SetDirection(dfDIR_RIGHT);
		SetSprite(e_SPRITE::ePLAYER_ATTACK3_R01, e_SPRITE::ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);
	}
}

void CPlayerObject::SetActionMove(DWORD action)
{
	// ���� �ȱ� ������ �ƴϾ��ų�, ������ �ٲ� �����
	// �̵� ��������Ʈ�� ���� �ٲ��ش�.
	if (!(dfACTION_MOVE_LL <= _dwActionCur && dfACTION_MOVE_LD >= _dwActionCur) || _iDirOld != _iDirCur)
	{
		// TODO : ����?
		//SetDirection(_iDirCur);

		if (dfDIR_LEFT == GetDirection())
		{
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		}
		else
		{
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}

		wcout << L"Set Action Move" << endl;
	}

	// TODO : ����?
	//_dwActionCur = action;
	_dwActionOld = _dwActionCur;
	_dwActionCur = action;
}

void CPlayerObject::SetActionStand(void)
{
	_dwActionOld = _dwActionCur;
	_dwActionCur = dfACTION_STAND;

	if (_dwActionCur == _dwActionOld)
	{
		return;
	}

	if (_iDirCur == dfDIR_LEFT)
	{
		SetSprite(e_SPRITE::ePLAYER_STAND_L01, e_SPRITE::ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	}
	else
	{
		SetSprite(e_SPRITE::ePLAYER_STAND_R01, e_SPRITE::ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	}
}