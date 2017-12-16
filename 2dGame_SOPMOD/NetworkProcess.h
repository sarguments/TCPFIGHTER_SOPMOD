#pragma once

class CRingBuffer;
class CBaseObject;
class CPacket;

extern SOCKET g_serverSock;
extern SOCKADDR_IN g_serverAddr;
extern CRingBuffer g_recvQ;
extern CRingBuffer g_sendQ;
extern WCHAR g_szIP[16];
extern HWND g_hWnd;
extern bool g_isConnected;
extern bool g_sendFlag;
extern CBaseObject* g_pPlayerObject;
extern std::list<CBaseObject*> g_ObjectList;

// SEND //
int SendEvent(void);
bool SendPacket(CPacket* packet);

// RECEV //
int ProcRead(void);
void RecvPacketProc(BYTE type, CPacket* packet);

// FUNC //
bool NetInit(void);
bool NetworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

////////// SC //////////
void SC_CREATE_MY_CHARACTER(CPacket* packet);
void SC_CREATE_OTHER_CHARACTER(CPacket* packet);
void SC_DELETE_CHARACTER(CPacket* packet);
void SC_MOVE_START(CPacket* packet);
void SC_MOVE_STOP(CPacket* packet);
void SC_ATTACK1(CPacket* packet);
void SC_ATTACK2(CPacket* packet);
void SC_ATTACK3(CPacket* packet);
void SC_DAMAGE(CPacket* packet);

////////// CS //////////
void CS_MOVE_START(CPacket* packet, BYTE dir, WORD x, WORD y);
void CS_MOVE_STOP(CPacket* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK1(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK1* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK2(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK2* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK3(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK3* packet, BYTE dir, WORD x, WORD y);