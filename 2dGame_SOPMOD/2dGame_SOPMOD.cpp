#include "stdafx.h"

#include "hoxy_Header.h"
#include "RingBuffer_AEK999.h"

#include "2dGame_SOPMOD.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"

#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
#include "CFrameSkip.h"

#include "GameInit.h"
#include "UpdateProcess.h"
#include "NetworkProcess.h"

// IME 삭제를 위함
#pragma comment(lib, "imm32.lib")
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;
HIMC g_hOldIMC;

WCHAR g_szIP[16] = {};
SOCKET g_serverSock = INVALID_SOCKET;
SOCKADDR_IN g_serverAddr;

bool g_isConnected;
bool g_sendFlag;

// 링버퍼
CRingBuffer g_recvQ(500);
CRingBuffer g_sendQ(500);

// dib 객체
CScreenDib g_ScreenDib(640, 480, 32);
CSpriteDib g_SpriteDib(e_SPRITE::eSPRITE_MAX, 0x00ffffff);
CFrameSkip g_FrameSkip(50);

// 플레이어 오브젝트 포인터
CBaseObject* g_pPlayerObject;

// 오브젝트 리스트
std::list<CBaseObject*> g_ObjectList;

// 창 활성화 여부
bool g_winActive;

// 게임
void Update(void);

// DIALOG //
BOOL CALLBACK DialogProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	CCmdStart myCmdStart;

	if (!GameInit())
	{
		MessageBox(g_hWnd, L"GameInit", L"ERROR", MB_OK);
		return FALSE;
	}

	int iReturn = DialogBox(hInstance, MAKEINTRESOURCE(IDD_ADDR), NULL, DialogProc);
	if (iReturn == FALSE)
	{
		MessageBox(g_hWnd, L"DialogBox", L"ERROR", MB_OK);
		DestroyWindow(g_hWnd);
	}

	CSockUtill::WSAStart();

	if (!NetInit())
	{
		MessageBox(g_hWnd, L"NetInit", L"ERROR", MB_OK);
		return FALSE;
	}

	MSG msg;

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2DGAMESOPMOD));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2DGAMESOPMOD);
	wcex.lpszClassName = L"TCP_FIGHTER";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(L"TCP_FIGHTER", NULL, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		MessageBox(g_hWnd, L"hWnd", L"ERROR", MB_OK);
		return FALSE;
	}

	g_hWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	RECT WindowRect;
	WindowRect.top = 0;
	WindowRect.left = 0;
	WindowRect.right = 640;
	WindowRect.bottom = 480;

	AdjustWindowRectEx(&WindowRect,
		GetWindowStyle(g_hWnd),
		// 메뉴가 있으면 1, 없으면 0
		GetMenu(g_hWnd) != NULL,
		GetWindowExStyle(g_hWnd));

	// 화면 크기 얻어서 정중앙으로
	int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
	int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

	MoveWindow(g_hWnd, iX, iY, WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top, TRUE);

	// IME 삭제
	g_hOldIMC = ImmAssociateContext(hWnd, NULL);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case UM_NETWORK:
	{
		if (!NetworkProc(hWnd, message, wParam, lParam))
		{
			CCmdStart::CmdDebugText(L"NetworkProc", false);
			MessageBox(g_hWnd, L"연결이 끊겼습니다.", L"TITLE : CLOSE", MB_OK);
			DestroyWindow(g_hWnd);
			return false;
		}
	}
	break;
	case WM_ACTIVATEAPP:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			g_winActive = false;
		}
		else
		{
			g_winActive = true;
		}
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		for (auto& i : g_ObjectList)
		{
			delete i;
		}

		g_ObjectList.clear();

		timeEndPeriod(1);
		ImmAssociateContext(hWnd, g_hOldIMC);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Update(void)
{
	if (g_winActive)
	{
		KeyProcess();
	}

	if (g_pPlayerObject == nullptr)
	{
		return;
	}

	Action();

	if (g_FrameSkip.FrameSkip())
	{
		Draw();
	}
}

BOOL CALLBACK DialogProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hEditBox;

	switch (iMsg)
	{
	case WM_INITDIALOG:
	{
		memset(g_szIP, 0, sizeof(WCHAR) * 16);
		hEditBox = GetDlgItem(hWnd, IDC_EDIT);
		SetWindowText(hEditBox, L"127.0.0.1");
		return TRUE;
	}
	break;
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDOK:
		{
			GetDlgItemText(hWnd, IDC_EDIT, g_szIP, 16);
			EndDialog(hWnd, 99939);
			return TRUE;
		}
		case IDCANCEL:
		{
			EndDialog(hWnd, 99940);
			return FALSE;
		}
		break;
		}
	}
	}
	return FALSE;
}