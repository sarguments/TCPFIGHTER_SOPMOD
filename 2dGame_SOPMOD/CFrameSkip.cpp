#include "stdafx.h"
#include "hoxy_Header.h"

#include "CScreenDib.h"
#include "CFrameSkip.h"

CFrameSkip::CFrameSkip(int iMaxFPS)
{
	_maxFPS = iMaxFPS; // �ִ� FPS(50)
	_oneFrameTick = 1000 / iMaxFPS; // �� �����Ӵ� Tick ��
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
		// ���� �ð��� ���� ����ð��� 1000ms ũ��
		if (_lastTime + 1000 < (int)timeGetTime())
		{
			WCHAR titleText[255];
			wsprintf(titleText, L"FPS : %d", _tickCount);

			HWND hWnd = FindWindow(L"TCP_FIGHTER", NULL);
			SetWindowText(hWnd, titleText);

			_tickCount = 0;

			// ������ ���� �ð� ���Ѵ�
			_lastTime = timeGetTime();
		}

		// ���� �ð� ���ϱ�
		nowTic = timeGetTime();

		// ����ð� - ������ ���������� �� ���� �ð� .. �� ���� ���� �ջ�
		_tickSum += nowTic - _oldTick;

		// �� �������� �ð��� �� �� ���.
		if (_tickSum < _oneFrameTick) // ���� ���� 20���� ������
		{
			// ���� ��ŭ ����
			Sleep(_oneFrameTick - _tickSum);

			nowTic = timeGetTime(); // ������ �� ����ð� �ٽ� �־��ش�
									// ���� ��ŭ ���� �����ϱ� 20 �־��ش�(���߿� ������ 20 ������)
			_tickSum = _oneFrameTick;

			byReturnFlag = true;
		}	// �� �������� �ð��� �Ѿ���� ���.
		else if (_tickSum - _oneFrameTick >= _oneFrameTick)
		{
			// ���� �������� _oneFrameTick���� ũ�� false
			byReturnFlag = false;
		}
		else
		{
			// ���� �Ѿ��� ���� true�� �ϴ� �Ѿ
			byReturnFlag = true;
		}
	}

	// ������������ �ð� ���ϱ�
	_oldTick = timeGetTime();

	// �Լ� ���������� 20������ �Һ�� �ɷ�
	// 20������ �� ������ �������� ��� ����
	_tickSum -= _oneFrameTick;

	if (byReturnFlag == true)
	{
		// ������ ǥ�ÿ� ī��Ʈ ����
		_tickCount++;
	}

	return byReturnFlag;
}