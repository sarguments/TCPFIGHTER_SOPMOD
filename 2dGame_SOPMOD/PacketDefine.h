﻿#pragma once

/*---------------------------------------------------------------

패킷데이터 정의.

자신의 캐릭터에 대한 패킷을 서버에게 보낼 때, 모두 자신이 먼저
액션을 취함과 동시에 패킷을 서버로 보내주도록 한다.

- 이동 키 입력 시 이동동작을 취함과 동시에 이동 패킷을 보내도록 한다.
- 공격키 입력 시 공격 동작을 취하면서 패킷을 보낸다.
- 충돌 처리 및 데미지에 대한 정보는 서버에서 처리 후 통보하게 된다.

---------------------------------------------------------------*/
#define UM_NETWORK (WM_USER + 1)

// TCP
#define SERVERPORT 5000
#define HEADERSIZE 4

/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 패킷헤더.
//
//---------------------------------------------------------------
/*
BYTE	Code;			// 패킷코드 0x89 고정.
BYTE	Size;			// 패킷 사이즈.  (실제 페이로드 사이즈, 헤더/End 코드 제외)
BYTE	Type;			// 패킷타입.
BYTE	Temp;			// 사용안함.
*/

#pragma pack(push, 1)
struct st_NETWORK_PACKET_HEADER
{
	BYTE	_Code;			// 패킷코드 0x89 고정.
	BYTE	_Size;			// 패킷 사이즈.
	BYTE	_Type;			// 패킷타입.
	BYTE	_Temp;			// 사용안함.
};
#pragma pack(pop)

//---------------------------------------------------------------
// 패킷의 가장 앞에 들어갈 패킷코드.
//---------------------------------------------------------------
#define dfNETWORK_PACKET_CODE	((BYTE)0x89)
//---------------------------------------------------------------
// 패킷의 끝 부분에는 1Byte 의 EndCode 가 포함된다.  0x79
//---------------------------------------------------------------
#define dfNETWORK_PACKET_END	((BYTE)0x79)

#define	dfPACKET_SC_CREATE_MY_CHARACTER			0
//---------------------------------------------------------------
// 0 - 클라이언트 자신의 캐릭터 할당		Server -> [Client]
//
// 서버에 접속시 최초로 받게되는 패킷으로 자신이 할당받은 ID 와
// 자신의 최초 위치, HP 를 받게 된다. (처음에 한번 받게 됨)
//
// 이 패킷을 받으면 자신의 ID,X,Y,HP 를 저장하고 캐릭터를 생성시켜야 한다.
// DWORD id, BYTE dir, WORD x, WORD y, BYTE hp
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_CREATE_MY_CHARACTER
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
	BYTE	_HP;
};
#pragma pack(pop)

#define	dfPACKET_SC_CREATE_OTHER_CHARACTER		1
//---------------------------------------------------------------
// 1. 다른 클라이언트의 캐릭터 생성 패킷		Server -> [Client]
//
// 처음 서버에 접속시 이미 접속되어 있던 캐릭터들의 정보
// 또는 게임중에 접속된 클라이언트들의 생성 용 정보.
//  DWORD id, BYTE dir, WORD x, WORD y, BYTE hp
//
//	4	-	ID
//	1	-	Direction
//	2	-	X
//	2	-	Y
//	1	-	HP
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_CREATE_OTHER_CHARACTER
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
	BYTE	_HP;
};
#pragma pack(pop)

#define	dfPACKET_SC_DELETE_CHARACTER			2
//---------------------------------------------------------------
// 2. 캐릭터 삭제 패킷						Server -> [Client]
//
// 캐릭터의 접속해제 또는 캐릭터가 죽었을때 전송됨.
//
//	4	-	ID
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_DELETE_CHARACTER
{
	DWORD	_ID;
};
#pragma pack(pop)

#define	dfPACKET_CS_MOVE_START					10
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 10. 캐릭터 이동시작 패킷						[Client] -> Server
//
// 자신의 캐릭터 이동시작시 이 패킷을 보낸다.
// 이동 중에는 본 패킷을 보내지 않으며, 키 입력이 변경되었을 경우에만
// 보내줘야 한다.
//
// (왼쪽 이동중 위로 이동 / 왼쪽 이동중 왼쪽 위로 이동... 등등)
// BYTE dir, WORD x, WORD y
//	1	-	Direction	( 방향 디파인 값 8방향 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_CS_MOVE_START
{
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define dfPACKET_MOVE_DIR_LL					0
#define dfPACKET_MOVE_DIR_LU					1
#define dfPACKET_MOVE_DIR_UU					2
#define dfPACKET_MOVE_DIR_RU					3
#define dfPACKET_MOVE_DIR_RR					4
#define dfPACKET_MOVE_DIR_RD					5
#define dfPACKET_MOVE_DIR_DD					6
#define dfPACKET_MOVE_DIR_LD					7

#define	dfPACKET_SC_MOVE_START					11
//---------------------------------------------------------------
// 캐릭터 이동시작 패킷						Server -> [Client]
//
// 다른 유저의 캐릭터 이동시 본 패킷을 받는다.
// 패킷 수신시 해당 캐릭터를 찾아 이동처리를 해주도록 한다.
//
// 패킷 수신 시 해당 키가 계속해서 눌린것으로 생각하고
// 해당 방향으로 계속 이동을 하고 있어야만 한다.
//
//	4	-	ID
//	1	-	Direction	( 방향 디파인 값 8방향 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_MOVE_START
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_CS_MOVE_STOP					12
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 캐릭터 이동중지 패킷						[Client] -> Server
//
// 이동중 키보드 입력이 없어서 정지되었을 때, 이 패킷을 서버에 보내준다.
//
//	1	-	Direction	( 방향 디파인 값 좌/우만 사용 )
//	2	-	X
//	2	-	Y
// BYTE dir, WORD x, WORD y
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_CS_MOVE_STOP
{
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_SC_MOVE_STOP					13
//---------------------------------------------------------------
// 캐릭터 이동중지 패킷						Server -> [Client]
//
// ID 에 해당하는 캐릭터가 이동을 멈춘것이므로
// 캐릭터를 찾아서 방향과, 좌표를 입력해주고 멈추도록 처리한다.
//
//	4	-	ID
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_MOVE_STOP
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_CS_ATTACK1						20
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 캐릭터 공격 패킷							[Client] -> Server
//
// 공격 키 입력시 본 패킷을 서버에게 보낸다.
// 충돌 및 데미지에 대한 결과는 서버에서 알려 줄 것이다.
//
// 공격 동작 시작시 한번만 서버에게 보내줘야 한다.
//
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
// BYTE dir, WORD x, WORD y
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_CS_ATTACK1
{
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_SC_ATTACK1						21
//---------------------------------------------------------------
// 캐릭터 공격 패킷							Server -> [Client]
//
// 패킷 수신시 해당 캐릭터를 찾아서 공격1번 동작으로 액션을 취해준다.
// 방향이 다를 경우에는 해당 방향으로 바꾼 후 해준다.
// DWORD id, BYTE dir, WORD x, WORD y
//	4	-	ID
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_ATTACK1
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_CS_ATTACK2						22
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 캐릭터 공격 패킷							[Client] -> Server
//
// 공격 키 입력시 본 패킷을 서버에게 보낸다.
// 충돌 및 데미지에 대한 결과는 서버에서 알려 줄 것이다.
//
// 공격 동작 시작시 한번만 서버에게 보내줘야 한다.
//
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_CS_ATTACK2
{
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_SC_ATTACK2						23
//---------------------------------------------------------------
// 캐릭터 공격 패킷							Server -> [Client]
//
// 패킷 수신시 해당 캐릭터를 찾아서 공격2번 동작으로 액션을 취해준다.
// 방향이 다를 경우에는 해당 방향으로 바꾼 후 해준다.
//
//	4	-	ID
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_ATTACK2
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_CS_ATTACK3						24
/////////////////////////////////////////////////////////////////
//---------------------------------------------------------------
// 캐릭터 공격 패킷							[Client] -> Server
//
// 공격 키 입력시 본 패킷을 서버에게 보낸다.
// 충돌 및 데미지에 대한 결과는 서버에서 알려 줄 것이다.
//
// 공격 동작 시작시 한번만 서버에게 보내줘야 한다.
//
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_CS_ATTACK3
{
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_SC_ATTACK3						25
//---------------------------------------------------------------
// 캐릭터 공격 패킷							Server -> [Client]
//
// 패킷 수신시 해당 캐릭터를 찾아서 공격3번 동작으로 액션을 취해준다.
// 방향이 다를 경우에는 해당 방향으로 바꾼 후 해준다.
//
//	4	-	ID
//	1	-	Direction	( 방향 디파인 값. 좌/우만 사용 )
//	2	-	X
//	2	-	Y
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_ATTACK3
{
	DWORD	_ID;
	BYTE	_Direction;
	WORD	_X;
	WORD	_Y;
};
#pragma pack(pop)

#define	dfPACKET_SC_DAMAGE						30
//---------------------------------------------------------------
// 캐릭터 데미지 패킷							Server -> [Client]
//
// 공격에 맞은 캐릭터의 정보를 보냄.
// DWORD AttackID, DWORD DmamgedID, BYTE DamagedHP
//	4	-	AttackID	( 공격자 ID )
//	4	-	DamageID	( 피해자 ID )
//	1	-	DamageHP	( 피해자 HP )
//
//---------------------------------------------------------------
#pragma pack(push, 1)
struct stPACKET_SC_DAMAGE
{
	DWORD _AttackID;
	DWORD _DamageID;
	BYTE _DamageHP;
};
#pragma pack(pop)