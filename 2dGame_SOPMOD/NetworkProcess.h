#pragma once

class CRingBuffer;
class CBaseObject;

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

//enum class e_dir
//{
//	LL,
//	LU,
//	UU,
//	RU,
//	RR,
//	RD,
//	DD,
//	LD
//};

// SEND //
int SendEvent(void);
bool SendPacket(st_NETWORK_PACKET_HEADER* header, char* packet);

// RECEV //
int ProcRead(void);
void RecvPacketProc(BYTE type, char* buf);

// FUNC //
bool NetInit(void);
bool NetworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

////////// SC //////////
void SC_CREATE_MY_CHARACTER(char* packet);
void SC_CREATE_OTHER_CHARACTER(char* packet);
void SC_DELETE_CHARACTER(char* packet);
void SC_MOVE_START(char* packet);
void SC_MOVE_STOP(char* packet);
void SC_ATTACK1(char* packet);
void SC_ATTACK2(char* packet);
void SC_ATTACK3(char* packet);
void SC_DAMAGE(char* packet);

////////// CS //////////
void CS_MOVE_START(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_MOVE_START* packet, BYTE dir, WORD x, WORD y);
void CS_MOVE_STOP(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_MOVE_STOP* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK1(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK1* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK2(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK2* packet, BYTE dir, WORD x, WORD y);
void CS_ATTACK3(st_NETWORK_PACKET_HEADER* header, stPACKET_CS_ATTACK3* packet, BYTE dir, WORD x, WORD y);