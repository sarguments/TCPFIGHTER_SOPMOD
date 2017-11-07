#pragma once

class CScreenDib;
class CSpriteDib;
class CBaseObject;

extern HWND g_hWnd;
extern CScreenDib g_ScreenDib;
extern CSpriteDib g_SpriteDib;

extern CBaseObject* g_pPlayerObject;
extern std::list<CBaseObject*> g_ObjectList;

bool GameInit(void);