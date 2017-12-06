#include "stdafx.h"
#include "hoxy_Header.h"

#include "CScreenDib.h"
#include "CFrameSkip.h"

CFrameSkip::CFrameSkip(int iMaxFPS)
{
	_maxFPS = iMaxFPS; // �ִ� FPS(50)
	_oneFrameTick = 1000 / iMaxFPS; // �� �����Ӵ� Tick ��
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

		// ���� �ð� ���ϱ�
		myTic = timeGetTime();

		// ����ð� - ������ ���������� �� ���� �ð� .. �� ���� ���� �ջ�
		_tick += myTic - _systemTick;

		// �� �������� �ð��� �� �� ���.
		if (_tick < _oneFrameTick) // ���� ���� 20���� ������
		{
			Sleep(_oneFrameTick - _tick);

			myTic = timeGetTime(); // ������ �� ����ð� �ٽ� �־��ش�

								   // ���� ��ŭ ���� �����ϱ� 20 �־��ش�(���߿� ������ 20 ������)
			_tick = _oneFrameTick; // m_iTick = 20

			byReturnFlag = true;
		}	// �� �������� �ð��� �Ѿ���� ���.
		else if (_tick - _oneFrameTick >= _oneFrameTick)
		{
			// ���� �������� m_iOneFrameTick���� ũ�� false
			byReturnFlag = false;
		}
		else
		{
			// ���� �Ѿ��� ���� true
			byReturnFlag = true;
		}
	}

	// ������������ �ð� ���ϱ�
	_systemTick = timeGetTime();

	// �Լ� ���������� 20������ �Һ�� �ɷ�
	// 20������ �� ������ �������� ��� ����
	_tick -= _oneFrameTick;

	if (byReturnFlag == true)
	{
		_tickCount++;
	}

	return byReturnFlag;
}