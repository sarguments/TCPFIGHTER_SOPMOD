#pragma once

class CFrameSkip
{
protected:

	int	_maxFPS;

	// ������ �ӵ� ������ ����ϰԵ� Tick ��. �������ӿ� ���Ǵ� Tick ��.
	// 1000 / m_iMaxFPS
	int	_oneFrameTick;

	// FrameSkip() �� ȣ���ϱ� �� timeGetTime ���� ���� Tick ��.
	DWORD _oldTick;

	int	_tickSum;
	int _tickCount;
	int _lastTime;

public:
	CFrameSkip(int iMaxFPS);
	~CFrameSkip();

	bool FrameSkip(void);
};