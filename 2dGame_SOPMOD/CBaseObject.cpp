#include "stdafx.h"
#include "hoxy_Header.h"

#include "CBaseObject.h"

// TODO : _dwActionInput �⺻�� 999
CBaseObject::CBaseObject()
	:_bSpriteEnd(false), _bEndFrame(false), _dwActionInput(999)
{
}

CBaseObject::~CBaseObject()
{
}

void CBaseObject::ActionInput(DWORD dwAction)
{
	_dwActionInput = dwAction;
}

int CBaseObject::GetCurX(void)
{
	return _iCurX;
}

int CBaseObject::GetCurY(void)
{
	return _iCurY;
}

int CBaseObject::GetObjectID(void)
{
	return _iObjectID;
}

int CBaseObject::GetObjectType(void)
{
	return _iObjectType;
}

int CBaseObject::GetOldX(void)
{
	return _iOldX;
}

int CBaseObject::GetOldY(void)
{
	return _iOldY;
}

int CBaseObject::GetSprite(void)
{
	return _iSpriteNow;
}

bool CBaseObject::isEndFrame(void)
{
	return _bEndFrame;
}

void CBaseObject::NextFrame(void)
{
	if (0 > _iSpriteStart)
	{
		return;
	}

	// ������ ������ ���� �Ѿ�� ���� ���������� �Ѿ
	++_iDelayCount;

	if (_iDelayCount >= _iFrameDelay)
	{
		// ī��Ʈ ���̸� ó������ �ǵ���
		_iDelayCount = 0;

		// ��������Ʈ ��������
		_iSpriteNow++;

		if (_iSpriteNow > _iSpriteMax)
		{
			_iSpriteNow = _iSpriteStart;
			_bEndFrame = true;
		}
	}
}

void CBaseObject::SetCurPosition(int x, int y)
{
	_iCurX = x;
	_iCurY = y;
}

void CBaseObject::SetObjectID(int param)
{
	_iObjectID = param;
}

void CBaseObject::SetObjectType(int param)
{
	_iObjectType = param;
}

void CBaseObject::SetOldPosition(int x, int y)
{
	_iOldX = x;
	_iOldY = y;
}

void CBaseObject::SetPosition(int x, int y)
{
	// ����Ʈ�� ���� ���� ����
	int objectType = GetObjectType();

	// ȭ�� �̵����� : ���� ������� �̵� �� ���� ������ �ɸ���
	//	���� Ÿ�� �̵��ϴ°� �ƴϸ�, ���ڸ��� ����� ��.
	if (
		(e_OBJECT_TYPE::eTYPE_EFFECT != objectType) && 
			((x <= dfRANGE_MOVE_LEFT) || (x >= dfRANGE_MOVE_RIGHT) ||
			(y <= dfRANGE_MOVE_TOP) || (y >= dfRANGE_MOVE_BOTTOM))
		)
	{
		return;
	}

	int curX = GetCurX();
	int curY = GetCurY();
	SetOldPosition(curX, curY);
	SetCurPosition(x, y);
}

void CBaseObject::SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay)
{
	_iSpriteStart = iSpriteStart;
	_iSpriteMax = iSpriteMax;
	_iFrameDelay = iFrameDelay;

	_iSpriteNow = iSpriteStart;
	_iDelayCount = 0;
	_bEndFrame = false;
}