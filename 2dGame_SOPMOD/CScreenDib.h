#pragma once

/*
DWORD - CHAR
0xARGB - B, G, R, A
*/

class CScreenDib
{
public:
	CScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDib();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer(void);

public:
	void DrawBuffer(HWND hWnd, int iX = 0, int iY = 0);

	BYTE* GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);

protected:
	BITMAPINFO _stDibInfo;
	BYTE* _bypBuffer;

	int _iWidth;
	int _iHeight;
	int _iPitch; // 4바이트 정렬된 한 줄의 바이트 수
	int _iColorBit;
	int _iBufferSize;
};