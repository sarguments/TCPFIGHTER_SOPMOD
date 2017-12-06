#include "stdafx.h"
#include "hoxy_Header.h"

#include "CScreenDib.h"
#include "CFrameSkip.h"

CFrameSkip::CFrameSkip(int iMaxFPS)
{
	_maxFPS = iMaxFPS; // 최대 FPS(50)
	_oneFrameTick = 1000 / iMaxFPS; // 한 프레임당 Tick 값
	_systemTick = timeGetTime();

	_tick = 0;
	_tickCount = 0;
	_oneSecond = timeGetTime();
}

CFrameSkip::~CFrameSkip()
{
}

bool CFrameSkip::FrameSkip()
{
	DWORD myTic;
	bool byReturnFlag = false;

	if (_tick < _oneFrameTick)
	{
		if (_oneSecond + 1000 < (int)timeGetTime())
		{
			WCHAR titleText[255];
			wsprintf(titleText, L"FPS : %d", _tickCount);

			HWND hWnd = FindWindow(L"TCP_FIGHTER", NULL);
			SetWindowText(hWnd, titleText);

			_tickCount = 0;
			_oneSecond = timeGetTime();
		}

		// 현재 시간 구하기
		myTic = timeGetTime();

		// 현재시간 - 이전에 빠져나오기 전 구한 시간 .. 의 차를 누적 합산
		_tick += myTic - _systemTick;

		// 한 프레임의 시간이 덜 된 경우.
		if (_tick < _oneFrameTick) // 누적 값이 20보다 작으면
		{
			Sleep(_oneFrameTick - _tick);

			myTic = timeGetTime(); // 슬립한 후 현재시간 다시 넣어준다

								   // 빠른 만큼 슬립 했으니까 20 넣어준다(나중에 무조건 20 뺄것임)
			_tick = _oneFrameTick; // m_iTick = 20

			byReturnFlag = true;
		}	// 한 프레임의 시간을 넘어버린 경우.
		else if (_tick - _oneFrameTick >= _oneFrameTick)
		{
			// 누적 프레임이 m_iOneFrameTick보다 크면 false
			byReturnFlag = false;
		}
		else
		{
			// 조금 넘었을 경우는 true
			byReturnFlag = true;
		}
	}

	// 빠져나오기전 시간 구하기
	_systemTick = timeGetTime();

	// 함수 빠져나오면 20프레임 소비된 걸로
	// 20프레임 뺀 나머지 누적값은 계속 보관
	_tick -= _oneFrameTick;

	if (byReturnFlag == true)
	{
		_tickCount++;
	}

	return byReturnFlag;
}