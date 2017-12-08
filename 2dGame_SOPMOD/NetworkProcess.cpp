#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "hoxy_Header.h"
#include "RingBuffer_AEK999.h"
#include "PacketDefine.h"
#include "CPlayerObject.h"

#include "NetworkProcess.h"

int ProcSend(void)
{
	return 0;
}

int SendPacket(char * buffer, int size)
{
	return 0;
}

int ProcRead(void)
{
	// recv 후 수신큐에 넣는다
	char localBuf[1000];
	int ret_recv = recv(g_serverSock, localBuf, 1000, 0);
	if (ret_recv == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CCmdStart::CmdDebugText(L"recv()", false);
			return -1;
		}
	}

	// 소켓종료 체크
	if (ret_recv == 0)
	{
		wcout << L"closesocket" << endl;
		return -1;
	}

	int ret_enqueue = g_recvQ.Enqueue(localBuf, ret_recv);
	if (ret_enqueue != ret_recv)
	{
		CCmdStart::CmdDebugText(L"Enqueue()", false);
		return -1;
	}

	while (1)
	{
		// 패킷 체크
		int retval = CheckPacket(&g_recvQ);
		if (retval < 0)
		{
			return -1;
		}
		else if (retval == 0)
		{
			// 아직 다 못받았으면 리턴 0
			return 0;
		}

		// 헤더 디큐 후 packetProc
		st_NETWORK_PACKET_HEADER localHeader;
		g_recvQ.Dequeue((char*)&localHeader, HEADERSIZE);
		RecvPacketProc(localHeader._Type);

		// 안쓰는 엔드코드 만큼 front 이동
		g_recvQ.MoveFrontPos(1);
	}

	return ret_recv;
}

int NetInit(void)
{
	g_serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_serverSock == INVALID_SOCKET)
	{
		CCmdStart::CmdDebugText(L"socket()", false);
	}

	g_serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, g_szIP, &g_serverAddr.sin_addr.s_addr);
	g_serverAddr.sin_port = htons(SERVERPORT);

	int ret_select = WSAAsyncSelect(g_serverSock, g_hWnd, UM_NETWORK, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (ret_select == SOCKET_ERROR)
	{
		CCmdStart::CmdDebugText(L"WSAAsyncSelect()", false);
		return -1;
	}

	int ret_connect = connect(g_serverSock, (SOCKADDR*)&g_serverAddr, sizeof(SOCKADDR));
	if (ret_connect != NOERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CCmdStart::CmdDebugText(L"connect()", false);
			return -1;
		}
	}

	return 0;
}

int NetworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		CCmdStart::CmdDebugText(L"WSAGETSELECTERROR", false);
		return -1;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
		// 통신을 위한 연결 절차가 끝났다.
	case FD_CONNECT:
	{
		// 연결 플래그
		g_isConnected = true;
	}
	break;
	case FD_WRITE:
	{
		// 처음 접속했을때 or 송신버퍼가 꽉찼다가 비워졌을때
		// WSAEWOULDBLOCK
		g_sendFlag = true;
	}
	break;
	case FD_READ:
	{
		ProcRead();
	}
	break;
	case FD_CLOSE:
	{
		closesocket(g_serverSock);
		g_isConnected = false;
		g_sendFlag = false;

		return -1;
	}
	break;
	}

	return 0;
}

int CheckPacket(CRingBuffer * buffer)
{
	CRingBuffer* pLocalBuffer = buffer;
	char peekBuf[100];

	int ret_peek = pLocalBuffer->Peek((char*)peekBuf, HEADERSIZE);
	// 헤더사이즈만큼 있는지
	if (ret_peek != HEADERSIZE)
	{
		return 0;
	}

	st_NETWORK_PACKET_HEADER* pLocalHeader = (st_NETWORK_PACKET_HEADER*)peekBuf;
	if (pLocalHeader->_Code != 0x89)
	{
		return -1;
	}

	// 페이로드 크기 + 헤더 + 엔드코드 크기 만큼 있으면 true
	if (buffer->GetUseSize() >= (pLocalHeader->_Size + HEADERSIZE + 1))
	{
		return 1;
	}

	return 0;
}

int RecvPacketProc(BYTE type)
{
	switch (type)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		SC_CREATE_MY_CHARACTER();
	}
	break;
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		SC_CREATE_OTHER_CHARACTER();
	}
	break;
	case dfPACKET_SC_DELETE_CHARACTER:
	{
		SC_DELETE_CHARACTER();
	}
	break;
	case dfPACKET_SC_MOVE_START:
	{
		SC_MOVE_START();
	}
	break;
	case dfPACKET_SC_MOVE_STOP:
	{
		SC_MOVE_STOP();
	}
	break;
	case dfPACKET_SC_ATTACK1:
	{
		// TODO : 방향 보정 해야함
		SC_ATTACK1();
	}
	break;
	case dfPACKET_SC_ATTACK2:
	{
		SC_ATTACK2();
	}
	break;
	case dfPACKET_SC_ATTACK3:
	{
		SC_ATTACK3();
	}
	break;
	case dfPACKET_SC_DAMAGE:
	{
		SC_DAMAGE();
	}
	break;
	}

	return 0;
}

void SC_CREATE_MY_CHARACTER(void)
{
	stPACKET_SC_CREATE_MY_CHARACTER localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_CREATE_MY_CHARACTER));

	CBaseObject* newObject = new CPlayerObject(localBuf._HP, true, localBuf._Direction);
	newObject->SetPosition(localBuf._X, localBuf._Y);
	newObject->SetObjectID(localBuf._ID);

	if (localBuf._Direction == dfDIR_LEFT)
	{
		newObject->SetSprite(e_SPRITE::ePLAYER_STAND_L01, e_SPRITE::ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	}
	else
	{
		newObject->SetSprite(e_SPRITE::ePLAYER_STAND_R01, e_SPRITE::ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	}

	g_pPlayerObject = newObject;
	g_ObjectList.push_back(newObject);
}

void SC_CREATE_OTHER_CHARACTER(void)
{
	stPACKET_SC_CREATE_OTHER_CHARACTER localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_CREATE_OTHER_CHARACTER));

	CBaseObject* newObject = new CPlayerObject(localBuf._HP, false, localBuf._Direction);
	newObject->SetPosition(localBuf._X, localBuf._Y);
	newObject->SetObjectID(localBuf._ID);

	wcout << L"_bPlayerCharacter = " << ((CPlayerObject*)newObject)->_bPlayerCharacter << endl;

	if (localBuf._Direction == dfDIR_LEFT)
	{
		newObject->SetSprite(e_SPRITE::ePLAYER_STAND_L01, e_SPRITE::ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	}
	else
	{
		newObject->SetSprite(e_SPRITE::ePLAYER_STAND_R01, e_SPRITE::ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	}

	g_ObjectList.push_back(newObject);
}

void SC_DELETE_CHARACTER(void)
{
	stPACKET_SC_DELETE_CHARACTER localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_DELETE_CHARACTER));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	DWORD localID = (*iter)->GetObjectID();

	delete (*iter);
	iter = g_ObjectList.erase(iter);

	wcout << L"SC_DELETE_CHARACTER ID : " << localID << endl;
}

void SC_MOVE_START(void)
{
	stPACKET_SC_MOVE_START localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_MOVE_START));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	switch (localBuf._Direction)
	{
		// TODO : X, Y 쓰나?
	case dfACTION_MOVE_LL:
	{
		(*iter)->ActionInput(dfACTION_MOVE_LL);
	}
	break;
	case dfACTION_MOVE_LU:
	{
		(*iter)->ActionInput(dfACTION_MOVE_LU);
	}
	break;
	case dfACTION_MOVE_UU:
	{
		(*iter)->ActionInput(dfACTION_MOVE_UU);
	}
	break;
	case dfACTION_MOVE_RU:
	{
		(*iter)->ActionInput(dfACTION_MOVE_RU);
	}
	break;
	case dfACTION_MOVE_RR:
	{
		(*iter)->ActionInput(dfACTION_MOVE_RR);
	}
	break;
	case dfACTION_MOVE_RD:
	{
		(*iter)->ActionInput(dfACTION_MOVE_RD);
	}
	break;
	case dfACTION_MOVE_DD:
	{
		(*iter)->ActionInput(dfACTION_MOVE_DD);
	}
	break;
	case dfACTION_MOVE_LD:
	{
		(*iter)->ActionInput(dfACTION_MOVE_LD);
	}
	break;
	}
}

void SC_MOVE_STOP(void)
{
	stPACKET_SC_MOVE_STOP localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_MOVE_STOP));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	(*iter)->ActionInput(dfACTION_STAND);
	wcout << L"________SC_MOVE_STOP________" << endl;
}

void SC_ATTACK1(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	(*iter)->ActionInput(dfACTION_ATTACK1);
}

void SC_ATTACK2(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	(*iter)->ActionInput(dfACTION_ATTACK2);
}

void SC_ATTACK3(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._ID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	(*iter)->ActionInput(dfACTION_ATTACK3);
}

void SC_DAMAGE(void)
{
	// TODO : 공격자 ID?
	stPACKET_SC_DAMAGE localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_DAMAGE));

	// 일치하는 아이디를 찾고
	auto iter = std::find_if(g_ObjectList.begin(), g_ObjectList.end(),
		[=](CBaseObject* param) {
		if (param->GetObjectType() == e_OBJECT_TYPE::eTYPE_PLAYER &&
			param->GetObjectID() == localBuf._DamageID)
		{
			return true;
		}
		return false;
	});

	if (iter == g_ObjectList.end())
	{
		wcout << L"SC_DELETE_CHARACTER FAIL" << endl;
		return;
	}

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	pPlayerObj->SetHP(localBuf._DamageHP);
}