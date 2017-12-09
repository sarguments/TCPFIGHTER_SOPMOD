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

enum class e_dir
{
	LL,
	LU,
	UU,
	RU,
	RR,
	RD,
	DD,
	LD
};

// SEND //
int ProcSend(void);
int SendPacket(char * buffer, int size);
int SendPacketProc(int inputParam);

// RECEV //
int ProcRead(void);
int RecvPacketProc(BYTE type);

// FUNC //
int NetInit(void);
int NetworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CheckPacket(CRingBuffer* buffer);

////////// SC //////////
void SC_CREATE_MY_CHARACTER(void);
void SC_CREATE_OTHER_CHARACTER(void);
void SC_DELETE_CHARACTER(void);
void SC_MOVE_START();
void SC_MOVE_STOP(void);
void SC_ATTACK1(void);
void SC_ATTACK2(void);
void SC_ATTACK3(void);
void SC_DAMAGE(void);

////////// CS //////////
void CS_MOVE_START(e_dir dir);
void CS_MOVE_STOP(void);
void CS_ATTACK1(void);
void CS_ATTACK2(void);
void CS_ATTACK3(void);