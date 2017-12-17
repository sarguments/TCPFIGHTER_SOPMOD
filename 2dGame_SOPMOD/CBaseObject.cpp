#include "stdafx.h"
#include "hoxy_Header.h"

#include "CBaseObject.h"

// TODO : _dwActionInput 기본값 999
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

	// 프레임 딜레이 값을 넘어야 다음 프레임으로 넘어감
	++_iDelayCount;

	if (_iDelayCount >= _iFrameDelay)
	{
		// 카운트 끝이면 처음으로 되돌림
		_iDelayCount = 0;

		// 스프라이트 다음으로
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
	// 이펙트의 경우는 제한 없음
	int objectType = GetObjectType();

	// 화면 이동영역 : 왼쪽 상단으로 이동 중 왼쪽 범위에 걸리면
	//	위로 타고 이동하는게 아니며, 그자리에 멈춰야 함.
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