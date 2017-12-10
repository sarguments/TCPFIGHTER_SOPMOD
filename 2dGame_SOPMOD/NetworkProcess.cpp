#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "hoxy_Header.h"
#include "RingBuffer_AEK999.h"
#include "PacketDefine.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"

#include "NetworkProcess.h"

int ProcSend(void)
{
	if (!g_isConnected)
	{
		CCmdStart::CmdDebugText(L"g_isConnected", false);
		return 0;
	}

	// sendFlag�� false�� sendQ�� �ϴ� �׾Ƴ��´�
	if (!g_sendFlag)
	{
		CCmdStart::CmdDebugText(L"g_sendFlag", false);
		return 0;
	}

	while (1)
	{
		// ����ť�� �ִ°� �� ������
		int inUseSize = g_sendQ.GetUseSize();
		if (inUseSize < HEADERSIZE)
		{
			break;
		}

		// ���� ����� ����
		st_NETWORK_PACKET_HEADER packetHeader;
		g_sendQ.Peek((char*)&packetHeader, HEADERSIZE);

		// ���̰� 0�� ���
		if (packetHeader._Size == 0)
		{
			return -1;
		}

		int packetSize = HEADERSIZE + packetHeader._Size + 1;

		// g_recvQ�� ��������� + ���̷ε� + �����ڵ�(1) ���� ��ŭ �ִ���
		if (g_sendQ.GetUseSize() < packetSize)
		{
			// ť�� ��ü ��Ŷ�� ���� �� �� ���� ���? (��� �޴´�)
			return 0;
		}

		// �ϴ� ���ؼ� �����ϰ� ����ť���� ������ŭ ������ ����
		char localBuf[1000];
		g_sendQ.Peek((char*)localBuf, packetSize);

		int ret_send = send(g_serverSock, localBuf, packetSize, 0);
		if (ret_send == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// TODO : ���� �׽�Ʈ
				// WOULDBLOCK �߸� sendFlag = false

				g_sendFlag = false;
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

int SendPacket(char * buffer, int size)
{
	// �ϴ� �־�� ���ϰ� != size �� ���� -1
	int ret_enqueue = g_sendQ.Enqueue(buffer, size);
	if (ret_enqueue != size)
	{
		CCmdStart::CmdDebugText(L"sendQ enqueue", false);
		return -1;
	}

	return ret_enqueue;
}

int SendPacketProc(int inputParam)
{
	switch (inputParam)
	{
	case dfACTION_MOVE_LL:
	{
		CS_MOVE_START(e_dir::LL);
		wcout << L"CS_MOVE_START(e_dir::LL);" << endl;
	}
	break;
	case dfACTION_MOVE_LU:
	{
		CS_MOVE_START(e_dir::LU);
		wcout << L"CS_MOVE_START(e_dir::LU);" << endl;
	}
	break;
	case dfACTION_MOVE_UU:
	{
		CS_MOVE_START(e_dir::UU);
		wcout << L"CS_MOVE_START(e_dir::UU);" << endl;
	}
	break;
	case dfACTION_MOVE_RU:
	{
		CS_MOVE_START(e_dir::RU);
		wcout << L"CS_MOVE_START(e_dir::RU);" << endl;
	}
	break;
	case dfACTION_MOVE_RR:
	{
		CS_MOVE_START(e_dir::RR);
		wcout << L"CS_MOVE_START(e_dir::RR);" << endl;
	}
	break;
	case dfACTION_MOVE_RD:
	{
		CS_MOVE_START(e_dir::RD);
		wcout << L"CS_MOVE_START(e_dir::RD);" << endl;
	}
	break;
	case dfACTION_MOVE_DD:
	{
		CS_MOVE_START(e_dir::DD);
		wcout << L"CS_MOVE_START(e_dir::DD);" << endl;
	}
	break;
	case dfACTION_MOVE_LD:
	{
		CS_MOVE_START(e_dir::LD);
		wcout << L"CS_MOVE_START(e_dir::LD);" << endl;
	}
	break;
	case dfACTION_ATTACK1:
	{
		CS_ATTACK1();
		wcout << L"CS_ATTACK1" << endl;
	}
	break;
	case dfACTION_ATTACK2:
	{
		CS_ATTACK2();
		wcout << L"CS_ATTACK2" << endl;
	}
	break;
	case dfACTION_ATTACK3:
	{
		CS_ATTACK3();
		wcout << L"CS_ATTACK3" << endl;
	}
	break;
	case dfACTION_STAND:
	{
		CS_MOVE_STOP();
		wcout << L"CS_MOVE_STOP" << endl;
	}
	break;
	}

	return 0;
}

int ProcRead(void)
{
	// recv �� ����ť�� �ִ´�
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

	// �������� üũ
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
		// ��Ŷ üũ
		int retval = CheckPacket(&g_recvQ);
		if (retval < 0)
		{
			return -1;
		}
		else if (retval == 0)
		{
			// ���� �� ���޾����� ���� 0
			return 0;
		}

		// ��� ��ť �� packetProc
		st_NETWORK_PACKET_HEADER localHeader;
		g_recvQ.Dequeue((char*)&localHeader, HEADERSIZE);
		RecvPacketProc(localHeader._Type);

		// �Ⱦ��� �����ڵ� ��ŭ front �̵�
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
		// ����� ���� ���� ������ ������.
	case FD_CONNECT:
	{
		// ���� �÷���
		g_isConnected = true;
	}
	break;
	case FD_WRITE:
	{
		// ó�� ���������� or �۽Ź��۰� ��á�ٰ� ���������
		// WSAEWOULDBLOCK
		g_sendFlag = true;

		int retval = ProcSend();
		if (retval == -1)
		{
			CCmdStart::CmdDebugText(L"ProcSend()", false);
			return -1;
		}
	}
	break;
	case FD_READ:
	{
		int retval = ProcRead();
		if (retval == -1)
		{
			CCmdStart::CmdDebugText(L"ProcRead()", false);
			return -1;
		}
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
	// ��������ŭ �ִ���
	if (ret_peek != HEADERSIZE)
	{
		return 0;
	}

	st_NETWORK_PACKET_HEADER* pLocalHeader = (st_NETWORK_PACKET_HEADER*)peekBuf;
	if (pLocalHeader->_Code != dfNETWORK_PACKET_CODE)
	{
		return -1;
	}

	// ���̷ε� ũ�� + ��� + �����ڵ� ũ�� ��ŭ ������ true
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

	// ��ġ�ϴ� ���̵� ã��
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

	// ��ġ�ϴ� ���̵� ã��
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
	// TODO : X, Y ����?
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

	// ��ġ�ϴ� ���̵� ã��
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

	wcout << L"________SC_MOVE_STOP________" << endl;
}

void SC_ATTACK1(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// ��ġ�ϴ� ���̵� ã��
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

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK1);
}

void SC_ATTACK2(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// ��ġ�ϴ� ���̵� ã��
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

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK2);
}

void SC_ATTACK3(void)
{
	stPACKET_SC_ATTACK1 localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_ATTACK1));

	// ��ġ�ϴ� ���̵� ã��
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

	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	int dir = pPlayerObj->GetDirection();
	if (dir != localBuf._Direction)
	{
		pPlayerObj->SetDirection(localBuf._Direction);
	}

	pPlayerObj->ActionInput(dfACTION_ATTACK3);
}

void SC_DAMAGE(void)
{
	stPACKET_SC_DAMAGE localBuf;
	g_recvQ.Dequeue((char*)&localBuf, sizeof(stPACKET_SC_DAMAGE));
	
	// TODO : �׽�Ʈ��
	char* pTemp = g_recvQ.GetFrontBufferPtr();

	// ��ġ�ϴ� ���̵� ã��
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

	// TODO : ����Ʈ �߰�
	CPlayerObject* pPlayerObj = (CPlayerObject*)(*iter);
	pPlayerObj->SetHP(localBuf._DamageHP);

	// TODO : ������ ������ �ǳ�?
	CBaseObject* newEffect = new CEffectObject(localBuf._AttackID, localBuf._DamageID);
	g_ObjectList.push_back(newEffect);

	wcout << L"****************New EFFECT" << endl;
}

void CS_MOVE_START(e_dir dir)
{
	//stPACKET_CS_MOVE_START
	// TODO : ��ť ���� ó�� �ؾߵǳ�?

	st_NETWORK_PACKET_HEADER localHeader;
	localHeader._Code = dfNETWORK_PACKET_CODE;
	localHeader._Size = sizeof(stPACKET_CS_MOVE_START);
	localHeader._Type = dfPACKET_CS_MOVE_START;
	SendPacket((char*)&localHeader, sizeof(st_NETWORK_PACKET_HEADER));

	stPACKET_CS_MOVE_START payload;
	switch (dir)
	{
	case e_dir::DD:
	{
		payload._Direction = dfACTION_MOVE_DD;
	}
	break;
	case e_dir::LD:
	{
		payload._Direction = dfACTION_MOVE_LD;
	}
	break;
	case e_dir::LL:
	{
		payload._Direction = dfACTION_MOVE_LL;
	}
	break;
	case e_dir::LU:
	{
		payload._Direction = dfACTION_MOVE_LU;
	}
	break;
	case e_dir::RD:
	{
		payload._Direction = dfACTION_MOVE_RD;
	}
	break;
	case e_dir::RR:
	{
		payload._Direction = dfACTION_MOVE_RR;
	}
	break;
	case e_dir::RU:
	{
		payload._Direction = dfACTION_MOVE_RU;
	}
	break;
	case e_dir::UU:
	{
		payload._Direction = dfACTION_MOVE_UU;
	}
	break;
	}
	payload._X = g_pPlayerObject->GetCurX();
	payload._Y = g_pPlayerObject->GetCurY();

	SendPacket((char*)&payload, sizeof(stPACKET_CS_MOVE_START));

	BYTE endCode = dfNETWORK_PACKET_END;
	SendPacket((char*)&endCode, sizeof(BYTE));
}

void CS_MOVE_STOP(void)
{
	//stPACKET_CS_MOVE_STOP
	st_NETWORK_PACKET_HEADER localHeader;
	localHeader._Code = dfNETWORK_PACKET_CODE;
	localHeader._Size = sizeof(stPACKET_CS_MOVE_STOP);
	localHeader._Type = dfPACKET_CS_MOVE_STOP;
	SendPacket((char*)&localHeader, sizeof(st_NETWORK_PACKET_HEADER));

	stPACKET_CS_MOVE_STOP payload;
	payload._X = g_pPlayerObject->GetCurX();
	payload._Y = g_pPlayerObject->GetCurY();
	payload._Direction = ((CPlayerObject*)g_pPlayerObject)->GetDirection();
	SendPacket((char*)&payload, sizeof(stPACKET_CS_MOVE_STOP));

	BYTE endCode = dfNETWORK_PACKET_END;
	SendPacket((char*)&endCode, sizeof(BYTE));
}

void CS_ATTACK1(void)
{
	//stPACKET_CS_ATTACK1
	st_NETWORK_PACKET_HEADER localHeader;
	localHeader._Code = dfNETWORK_PACKET_CODE;
	localHeader._Size = sizeof(stPACKET_CS_ATTACK1);
	localHeader._Type = dfPACKET_CS_ATTACK1;
	SendPacket((char*)&localHeader, sizeof(st_NETWORK_PACKET_HEADER));

	stPACKET_CS_ATTACK1 payload;
	payload._X = g_pPlayerObject->GetCurX();
	payload._Y = g_pPlayerObject->GetCurY();
	payload._Direction = ((CPlayerObject*)g_pPlayerObject)->GetDirection();
	SendPacket((char*)&payload, sizeof(stPACKET_CS_ATTACK1));

	BYTE endCode = dfNETWORK_PACKET_END;
	SendPacket((char*)&endCode, sizeof(BYTE));
}

void CS_ATTACK2(void)
{
	//stPACKET_CS_ATTACK2
	st_NETWORK_PACKET_HEADER localHeader;
	localHeader._Code = dfNETWORK_PACKET_CODE;
	localHeader._Size = sizeof(stPACKET_CS_ATTACK2);
	localHeader._Type = dfPACKET_CS_ATTACK2;
	SendPacket((char*)&localHeader, sizeof(st_NETWORK_PACKET_HEADER));

	stPACKET_CS_ATTACK2 payload;
	payload._X = g_pPlayerObject->GetCurX();
	payload._Y = g_pPlayerObject->GetCurY();
	payload._Direction = ((CPlayerObject*)g_pPlayerObject)->GetDirection();
	SendPacket((char*)&payload, sizeof(stPACKET_CS_ATTACK2));

	BYTE endCode = dfNETWORK_PACKET_END;
	SendPacket((char*)&endCode, sizeof(BYTE));
}

void CS_ATTACK3(void)
{
	// stPACKET_CS_ATTACK3
	st_NETWORK_PACKET_HEADER localHeader;
	localHeader._Code = dfNETWORK_PACKET_CODE;
	localHeader._Size = sizeof(stPACKET_CS_ATTACK3);
	localHeader._Type = dfPACKET_CS_ATTACK3;
	SendPacket((char*)&localHeader, sizeof(st_NETWORK_PACKET_HEADER));

	stPACKET_CS_ATTACK3 payload;
	payload._X = g_pPlayerObject->GetCurX();
	payload._Y = g_pPlayerObject->GetCurY();
	payload._Direction = ((CPlayerObject*)g_pPlayerObject)->GetDirection();
	SendPacket((char*)&payload, sizeof(stPACKET_CS_ATTACK3));

	BYTE endCode = dfNETWORK_PACKET_END;
	SendPacket((char*)&endCode, sizeof(BYTE));
}
