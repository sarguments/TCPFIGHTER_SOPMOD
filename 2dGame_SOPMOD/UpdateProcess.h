#pragma once

class CScreenDib;
class CSpriteDib;
class CBaseObject;

extern HWND g_hWnd;
extern CScreenDib g_ScreenDib;
extern CSpriteDib g_SpriteDib;

extern CBaseObject* g_pPlayerObject;

extern int g_xPos;
extern int g_yPos;

extern bool g_winActive;

void KeyProcess(void);
void Action(void);
//void FrameSkip(void);
void Draw(void);