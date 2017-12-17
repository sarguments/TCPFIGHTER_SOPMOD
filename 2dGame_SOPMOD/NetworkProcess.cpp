#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "hoxy_Header.h"
#include "RingBuffer_AEK999.h"
#include "PacketDefine.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"

#include "NetworkProcess.h"
#include "SerializeBuffer_CZ75.h"

int SendEvent(void)
{
	// sendFlag가 false면 sendQ에 일단 쌓아놓는다
	if (!g_sendFlag)
	{
		CCmdStart::CmdDebugText(L"g_sendFlag", false);
		return 0;
	}

	while (1)
	{
		int UseSize = g_sendQ.GetUseSize();

		// 보낼 데이터가 1바이트라도 있는지
		if (UseSize < 1)
		{
			// 큐에 전체 패킷이 아직 다 못 들어온 경우? (계속 받는다)
			return 0;
		}

		// 센드큐에서 보낸만큼 데이터 날림
		int ret_send = send(g_serverSock, g_sendQ.GetFrontBufferPtr(), UseSize, 0);
		if (ret_send == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// TODO : 우드블럭 테스트
				// WOULDBLOCK 뜨면 sendFlag = false

				g_sendFlag = false;
				return 0;
			}
			else
			{
				CCmdStart::CmdDebugText(L"send", false);

				return -1;
			}
		}

		//wcout << L"sendQ Use : " << inUseSize << L" / ret_send : " << ret_send << endl;
		g_sendQ.MoveFrontPos(ret_send);
	}

	return 0;
}

bool SendPacket(CPacket* packet)
{
	if (!g_isConnected)
	{
		CCmdStart::CmdDebugText(L"g_isConnected", false);
		return false;
	}

	// 일단 넣어보고 리턴값 != size 면 리턴 -1
	int ret_enqueue = g_sendQ.Enqueue(packet->GetBufferPtr(), packet->GetDataSize());
	if (ret_enqueue != packet->GetDataSize())
	{
		CCmdStart::CmdDebugText(L"sendQ enqueue", false);
		return false;
	}

	return true;
}

int ProcRead(void)
{
	// 수신큐에 리시브
	int ret_recv = recv(g_serverSock, g_recvQ.GetRearBufferPtr(), g_recvQ.GetFreeSize(), 0);
	if (ret_recv == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CCmdStart::CmdDebugText(L"recv()", false);
			return -1;
		}
	}

	g_recvQ.MoveRearPos(ret_recv);

	// 소켓종료 체크
	if (ret_recv == 0)
	{
		wcout << L"closesocket" << endl;
		return -1;
	}

	while (1)
	{
		char tempHeaderBuf[1000];

		// 헤더사이즈만큼 있는지
		int ret_peek = g_recvQ.Peek((char*)tempHeaderBuf, HEADERSIZE);
		if (ret_peek != HEADERSIZE)
		{
			return 0;
		}

		st_NETWORK_PACKET_HEADER* pLocalHeader = (st_NETWORK_PACKET_HEADER*)tempHeaderBuf;
		if (pLocalHeader->_Code != dfNETWORK_PACKET_CODE)
		{
			return -1;
		}

		// 페이로드 크기 + 헤더 + 엔드코드 크기 만큼 있나
		if (g_recvQ.GetUseSize() < (pLocalHeader->_Size + HEADERSIZE + 1))
		{
			return 0;
		}

		// peek 했으니까 디큐 다시 하지말고 그만큼 HEADERSIZE 이동
		g_recvQ.MoveFrontPos(HEADERSIZE);

		// 리시브 큐에 받아서 직렬화버퍼에 넣고 빼서 쓴다.
		CPacket packetBuf;

		// 페이로드 크기 만큼 디큐
		int ret_dequeue = g_recvQ.Dequeue(packetBuf.GetBufferPtr(), pLocalHeader->_Size);
		if (ret_dequeue != pLocalHeader->_Size)
		{
			CCmdStart::CmdDebugText(L"Dequeue()", false);
			return -1;
		}

		// 디큐 된 만큼 직렬화버퍼 rear 이동
		packetBuf.MoveRearPos(ret_dequeue);

		// 안쓰는 엔드코드 만큼 front 이동
		g_recvQ.MoveFrontPos(1);

		// 패킷 처리
		RecvPacketProc(pLocalHeader->_Type, &packetBuf);
	}

	return ret_recv;
}

bool NetInit(void)
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
		return false;
	}

	int ret_connect = connect(g_serverSock, (SOCKADDR*)&g_serverAddr, sizeof(SOCKADDR));
	if (ret_connect != NOERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			CCmdStart::CmdDebugText(L"connect()", false);
			return false;
		}
	}

	return true;
}

bool NetworkProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		CCmdStart::CmdDebugText(L"WSAGETSELECTERROR", false);
		return false;
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

		int retval = SendEvent();
		if (retval == -1)
		{
			CCmdStart::CmdDebugText(L"ProcSend()", false);
			return false;
		}
	}
	break;
	case FD_READ:
	{
		int retval = ProcRead();
		if (retval == -1)
		{
			CCmdStart::CmdDebugText(L"ProcRead()", false);
			return false;
		}
	}
	break;
	case FD_CLOSE:
	{
		closesocket(g_serverSock);
		g_isConnected = false;
		g_sendFlag = false;

		return false;
	}
	break;
	}

	return true;
}

void RecvPacketProc(BYTE type, CPacket* packet)
{
	switch (type)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		SC_CREATE_MY_CHARACTER(packet);
	}
	break;
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		SC_CREATE_OTHER_CHARACTER(packet);
	}
	break;
	case dfPACKET_SC_DELETE_CHARACTER:
	{
		SC_DELETE_CHARACTER(packet);
	}
	break;
	case dfPACKET_SC_MOVE_START:
	{
		SC_MOVE_START(packet);
	}
	break;
	case dfPACKET_SC_MOVE_STOP:
	{
		SC_MOVE_STOP(packet);
	}
	break;
	case dfPACKET_SC_ATTACK1:
	{
		SC_ATTACK1(packet);
	}
	break;
	case dfPACKET_SC_ATTACK2:
	{
		SC_ATTACK2(packet);
	}
	break;
	case dfPACKET_SC_ATTACK3:
	{
		SC_ATTACK3(packet);
	}
	break;
	case dfPACKET_SC_DAMAGE:
	{
		SC_DAMAGE(packet);
	}
	break;
	default:
	{
		CCmdStart::CmdDebugText(L"Fatal ERROR!!!!", false);

		// TODO : 크래쉬
		int* crash = nullptr;
		*crash = 999;

		return;
	}
	}
}

void SC_CREATE_MY_CHARACTER(CPacket* packet)
{
	stPACKET_SC_CREATE_MY_CHARACTER localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;
	(*packet) >> localBuf._HP;

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

void SC_CREATE_OTHER_CHARACTER(CPacket* packet)
{
	stPACKET_SC_CREATE_OTHER_CHARACTER localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;
	(*packet) >> localBuf._HP;

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

void SC_DELETE_CHARACTER(CPacket* packet)
{
	stPACKET_SC_DELETE_CHARACTER localBuf;
	(*packet) >> localBuf._ID;

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

void SC_MOVE_START(CPacket* packet)
{
	stPACKET_SC_MOVE_START localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;

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

	// TODO : x, y 역할이 없는데?
	(*iter)->SetPosition(localBuf._X, localBuf._Y);

	switch (localBuf._Direction)
	{
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

void SC_MOVE_STOP(CPacket* packet)
{
	stPACKET_SC_MOVE_STOP localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;

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

	CPlayerObject* pBaseObj = (CPlayerObject*)(*iter);
	pBaseObj->SetDirection(localBuf._Direction);
	pBaseObj->SetPosition(localBuf._X, localBuf._Y);
	pBaseObj->ActionInput(dfACTION_STAND);

	//wcout << L"________SC_MOVE_STOP________" << endl;
}

void SC_ATTACK1(CPacket* packet)
{
	stPACKET_SC_ATTACK1 localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;

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

	// TODO : x, y ?
	(*iter)->SetPosition(localBuf._X, localBuf._Y);

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK1);
}

void SC_ATTACK2(CPacket* packet)
{
	stPACKET_SC_ATTACK2 localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;

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

	// TODO : x, y ?
	(*iter)->SetPosition(localBuf._X, localBuf._Y);

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK2);
}

void SC_ATTACK3(CPacket* packet)
{
	stPACKET_SC_ATTACK3 localBuf;
	(*packet) >> localBuf._ID;
	(*packet) >> localBuf._Direction;
	(*packet) >> localBuf._X;
	(*packet) >> localBuf._Y;

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

	// TODO : x, y ?
	(*iter)->SetPosition(localBuf._X, localBuf._Y);

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK3);
}

void SC_DAMAGE(CPacket* packet)
{
	stPACKET_SC_DAMAGE localBuf;
	(*packet) >> localBuf._AttackID;
	(*packet) >> localBuf._DamageID;
	(*packet) >> localBuf._DamageHP;

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

	// 데미지 적용
	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	pPlayerObj->SetHP(localBuf._DamageHP);

	// 이펙트 추가
	CBaseObject* newEffect = new CEffectObject(localBuf._AttackID, localBuf._DamageID);
	g_ObjectList.push_back(newEffect);

	wcout << L"****************New EFFECT" << endl;
}

void CS_MOVE_START(CPacket* packet, BYTE dir, WORD x, WORD y)
{
	(*packet) << (BYTE)dfNETWORK_PACKET_CODE;
	(*packet) << (BYTE)sizeof(stPACKET_CS_MOVE_START);
	(*packet) << (BYTE)dfPACKET_CS_MOVE_START;
	(*packet) << (BYTE)0x00;

	(*packet) << dir;
	(*packet) << x;
	(*packet) << y;
}

void CS_MOVE_STOP(CPacket* packet, BYTE dir, WORD x, WORD y)
{
	(*packet) << (BYTE)dfNETWORK_PACKET_CODE;
	(*packet) << (BYTE)sizeof(stPACKET_CS_MOVE_STOP);
	(*packet) << (BYTE)dfPACKET_CS_MOVE_STOP;
	(*packet) << (BYTE)0x00;

	(*packet) << dir;
	(*packet) << x;
	(*packet) << y;
}

void CS_ATTACK1(CPacket* packet, BYTE dir, WORD x, WORD y)
{
	(*packet) << (BYTE)dfNETWORK_PACKET_CODE;
	(*packet) << (BYTE)sizeof(stPACKET_CS_ATTACK1);
	(*packet) << (BYTE)dfPACKET_CS_ATTACK1;
	(*packet) << (BYTE)0x00;

	(*packet) << dir;
	(*packet) << x;
	(*packet) << y;
}

void CS_ATTACK2(CPacket* packet, BYTE dir, WORD x, WORD y)
{
	(*packet) << (BYTE)dfNETWORK_PACKET_CODE;
	(*packet) << (BYTE)sizeof(stPACKET_CS_ATTACK2);
	(*packet) << (BYTE)dfPACKET_CS_ATTACK2;
	(*packet) << (BYTE)0x00;

	(*packet) << dir;
	(*packet) << x;
	(*packet) << y;
}

void CS_ATTACK3(CPacket* packet, BYTE dir, WORD x, WORD y)
{
	(*packet) << (BYTE)dfNETWORK_PACKET_CODE;
	(*packet) << (BYTE)sizeof(stPACKET_CS_ATTACK3);
	(*packet) << (BYTE)dfPACKET_CS_ATTACK3;
	(*packet) << (BYTE)0x00;

	(*packet) << dir;
	(*packet) << x;
	(*packet) << y;
}