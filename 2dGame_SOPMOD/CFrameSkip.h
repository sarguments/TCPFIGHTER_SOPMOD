#pragma once

class CFrameSkip
{
public:
	CFrameSkip();
	~CFrameSkip();
	bool FrameSkip(void);

private:
	//DWORD _dwSystemTick;
	//int _iMaxFPS;
	//int _iOneFrameTick;
	//int _iTick;

	LARGE_INTEGER _freq;
	LARGE_INTEGER _nowTick;
	LARGE_INTEGER _oldTick;
	__int64 _diffSum;
	int _tickCount;

	LARGE_INTEGER _oneSecond;
};