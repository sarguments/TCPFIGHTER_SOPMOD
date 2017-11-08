#pragma once

class CFrameSkip
{
protected:

	int	_maxFPS;

	// ������ �ӵ� ������ ����ϰԵ� Tick ��. �������ӿ� ���Ǵ� Tick ��.
	// 1000 / m_iMaxFPS
	int	_oneFrameTick;

	// FrameSkip() �� ȣ���ϱ� �� timeGetTime ���� ���� Tick ��.
	DWORD _systemTick;

	int	_tick;
	int _tickCount;
	int _oneSecond;

public:
	CFrameSkip(int iMaxFPS);
	~CFrameSkip();

	bool FrameSkip(void);
};