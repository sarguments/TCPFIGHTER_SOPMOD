#pragma once

class CFrameSkip
{
protected:

	int	_maxFPS;

	// 프레임 속도 조절시 사용하게될 Tick 값. 한프레임에 사용되는 Tick 값.
	// 1000 / m_iMaxFPS
	int	_oneFrameTick;

	// FrameSkip() 을 호출하기 전 timeGetTime 으로 얻은 Tick 값.
	DWORD _systemTick;

	int	_tick;
	int _tickCount;
	int _oneSecond;

public:
	CFrameSkip(int iMaxFPS);
	~CFrameSkip();

	bool FrameSkip(void);
};