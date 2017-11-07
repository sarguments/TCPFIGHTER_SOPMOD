#include "stdafx.h"
#include "CScreenDib.h"

CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
	: _iWidth(iWidth), _iHeight(iHeight), _iColorBit(iColorBit)
{
	// 스크린 버퍼 생성 함수 호출
	CreateDibBuffer(iWidth, iHeight, iColorBit);
}

CScreenDib::~CScreenDib()
{
	ReleaseDibBuffer();
}

void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	// 입력받은 인자의 정보로 멤버변수 정보 셋팅
	// BITMAPINFO(BITMAPINFOHEADER)를 셋팅한다.
	_stDibInfo.bmiHeader.biBitCount = iColorBit;
	_stDibInfo.bmiHeader.biCompression = BI_RGB;
	_stDibInfo.bmiHeader.biWidth = iWidth;
	_stDibInfo.bmiHeader.biHeight = -iHeight; // 높이 마이너스
	_stDibInfo.bmiHeader.biPlanes = 1;
	_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// 이미지 사이즈를 계산하여 버퍼용 이미지 동적 할당
	int WidthByte = iWidth * (iColorBit / 8);
	_iPitch = (WidthByte + 3) & ~3;
	_stDibInfo.bmiHeader.biSizeImage = _iPitch * iHeight;
	_bypBuffer = (BYTE*)malloc(_stDibInfo.bmiHeader.biSizeImage);
}

void CScreenDib::ReleaseDibBuffer(void)
{
	free(_bypBuffer);
}

void CScreenDib::DrawBuffer(HWND hWnd, int iX, int iY)
{
	HDC hdc = GetDC(hWnd);

	int retval = StretchDIBits(hdc, iX, iY, _iWidth, _iHeight,
		0, 0, _iWidth, _iHeight,
		_bypBuffer, &_stDibInfo, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(hWnd, hdc);
}

BYTE * CScreenDib::GetDibBuffer(void)
{
	return _bypBuffer;
}

int CScreenDib::GetWidth(void)
{
	return _iWidth;
}

int CScreenDib::GetHeight(void)
{
	return _iHeight;
}

int CScreenDib::GetPitch(void)
{
	return _iPitch;
}