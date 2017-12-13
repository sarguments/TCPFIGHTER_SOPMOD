#include "stdafx.h"
#include "hoxy_Header.h"

#include "CScreenDib.h"
#include "CFrameSkip.h"

CFrameSkip::CFrameSkip(int iMaxFPS)
{
	_maxFPS = iMaxFPS; // 최대 FPS(50)
	_oneFrameTick = 1000 / iMaxFPS; // 한 프레임당 Tick 값
	_oldTick = timeGetTime();

	_tickSum = 0;
	_tickCount = 0;
	_lastTime = timeGetTime();
}

CFrameSkip::~CFrameSkip()
{
}

bool CFrameSkip::FrameSkip()
{
	DWORD nowTic;
	bool byReturnFlag = false;

	if (_tickSum < _oneFrameTick)
	{
		// 이전 시간에 비해 현재시간이 1000ms 크면
		if (_lastTime + 1000 < (int)timeGetTime())
		{
			WCHAR titleText[255];
			wsprintf(titleText, L"FPS : %d", _tickCount);

			HWND hWnd = FindWindow(L"TCP_FIGHTER", NULL);
			SetWindowText(hWnd, titleText);

			_tickCount = 0;

			// 나가기 전에 시간 구한다
			_lastTime = timeGetTime();
		}

		// 현재 시간 구하기
		nowTic = timeGetTime();

		// 현재시간 - 이전에 빠져나오기 전 구한 시간 .. 의 차를 누적 합산
		_tickSum += nowTic - _oldTick;

		// 한 프레임의 시간이 덜 된 경우.
		if (_tickSum < _oneFrameTick) // 누적 값이 20보다 작으면
		{
			// 빠른 만큼 슬립
			Sleep(_oneFrameTick - _tickSum);

			nowTic = timeGetTime(); // 슬립한 후 현재시간 다시 넣어준다
									// 빠른 만큼 슬립 했으니까 20 넣어준다(나중에 무조건 20 뺄것임)
			_tickSum = _oneFrameTick;

			byReturnFlag = true;
		}	// 한 프레임의 시간을 넘어버린 경우.
		else if (_tickSum - _oneFrameTick >= _oneFrameTick)
		{
			// 누적 프레임이 _oneFrameTick보다 크면 false
			byReturnFlag = false;
		}
		else
		{
			// 조금 넘었을 경우는 true로 일단 넘어감
			byReturnFlag = true;
		}
	}

	// 빠져나오기전 시간 구하기
	_oldTick = timeGetTime();

	// 함수 빠져나오면 20프레임 소비된 걸로
	// 20프레임 뺀 나머지 누적값은 계속 보관
	_tickSum -= _oneFrameTick;

	if (byReturnFlag == true)
	{
		// 프레임 표시용 카운트 증가
		_tickCount++;
	}

	return byReturnFlag;
}