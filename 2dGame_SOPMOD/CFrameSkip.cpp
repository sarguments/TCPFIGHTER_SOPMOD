#include "stdafx.h"
#include "CFrameSkip.h"

CFrameSkip::CFrameSkip()
	: _diffSum(0), _tickCount(0)
{
	// ���ػ� Ÿ�̸��� ���ļ��� ��´�.
	QueryPerformanceFrequency(&_freq);
	QueryPerformanceCounter(&_oldTick);
}

CFrameSkip::~CFrameSkip()
{
}

bool CFrameSkip::FrameSkip(void)
{
	// 1�ʿ� 50�� ����
	// 1�� = 1000ms, 1000 / 50 = 20
	// �ѹ����µ� 20ms�ɷ��� �Ѵ�.

	// TODO : ������ ��ŵ FPS, ǥ�� ������ ���
	LARGE_INTEGER nowTime;
	QueryPerformanceCounter(&nowTime);

	if ((_oneSecond.QuadPart / (_freq.QuadPart / 1000)) + 1000 <
		(nowTime.QuadPart / (_freq.QuadPart / 1000)))
	{
		WCHAR titleText[100];
		wsprintf(titleText, L"FPS : %d", _tickCount);

		HWND hWnd = FindWindow(L"TCP_FIGHTER", NULL);
		SetWindowText(hWnd, titleText);

		_tickCount = 0;
		QueryPerformanceCounter(&_oneSecond);
	}

	// FrameSkip
	QueryPerformanceCounter(&_nowTick);
	_diffSum += (_nowTick.QuadPart - _oldTick.QuadPart) / (_freq.QuadPart / 1000);

	if (_diffSum < 20)
	{
		WCHAR toDebugText[20];
		wsprintf(toDebugText, L"< 20 : %d\n", _diffSum);
		OutputDebugString(toDebugText);

		Sleep(static_cast<DWORD>(20 - _diffSum));
		_diffSum = 0;

		// true�϶� ī��Ʈ
		++_tickCount;

		QueryPerformanceCounter(&_oldTick);
	}
	else
	{
		WCHAR toDebugText[20];
		wsprintf(toDebugText, L"> 20 = %d\n", _diffSum);
		OutputDebugString(toDebugText);

		_diffSum -= 20;

		QueryPerformanceCounter(&_oldTick);
		return false;
	}

	return true;
}