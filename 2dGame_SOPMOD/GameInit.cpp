#include "stdafx.h"
#include "hoxy_Header.h"

#include "GameInit.h"

#include "CScreenDib.h"
#include "CSpriteDib.h"

#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"

bool GameInit(void)
{
	if (!g_SpriteDib.LoadDibSprite(eMAP, L"Data\\_Map.bmp", 0, 0))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L01, L"Data\\Stand_L_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L02, L"Data\\Stand_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L03, L"Data\\Stand_L_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L04, L"Data\\Stand_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L05, L"Data\\Stand_L_01.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R01, L"Data\\Stand_R_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R02, L"Data\\Stand_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R03, L"Data\\Stand_R_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R04, L"Data\\Stand_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R05, L"Data\\Stand_R_01.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L01, L"Data\\Move_L_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L02, L"Data\\Move_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L03, L"Data\\Move_L_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L04, L"Data\\Move_L_04.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L05, L"Data\\Move_L_05.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L06, L"Data\\Move_L_06.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L07, L"Data\\Move_L_07.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L08, L"Data\\Move_L_08.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L09, L"Data\\Move_L_09.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L10, L"Data\\Move_L_10.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L11, L"Data\\Move_L_11.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L12, L"Data\\Move_L_12.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R01, L"Data\\Move_R_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R02, L"Data\\Move_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R03, L"Data\\Move_R_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R04, L"Data\\Move_R_04.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R05, L"Data\\Move_R_05.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R06, L"Data\\Move_R_06.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R07, L"Data\\Move_R_07.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R08, L"Data\\Move_R_08.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R09, L"Data\\Move_R_09.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R10, L"Data\\Move_R_10.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R11, L"Data\\Move_R_11.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R12, L"Data\\Move_R_12.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L01, L"Data\\Attack1_L_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L02, L"Data\\Attack1_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L03, L"Data\\Attack1_L_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L04, L"Data\\Attack1_L_04.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R01, L"Data\\Attack1_R_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R02, L"Data\\Attack1_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R03, L"Data\\Attack1_R_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R04, L"Data\\Attack1_R_04.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L01, L"Data\\Attack2_L_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L02, L"Data\\Attack2_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L03, L"Data\\Attack2_L_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L04, L"Data\\Attack2_L_04.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R01, L"Data\\Attack2_R_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R02, L"Data\\Attack2_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R03, L"Data\\Attack2_R_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R04, L"Data\\Attack2_R_04.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L01, L"Data\\Attack3_L_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L02, L"Data\\Attack3_L_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L03, L"Data\\Attack3_L_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L04, L"Data\\Attack3_L_04.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L05, L"Data\\Attack3_L_05.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L06, L"Data\\Attack3_L_06.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R01, L"Data\\Attack3_R_01.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R02, L"Data\\Attack3_R_02.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R03, L"Data\\Attack3_R_03.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R04, L"Data\\Attack3_R_04.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R05, L"Data\\Attack3_R_05.bmp", 71, 90))
		return false;
	if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R06, L"Data\\Attack3_R_06.bmp", 71, 90))
		return false;

	if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_01, L"Data\\xSpark_1.bmp", 70, 70))
		return false;
	if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_02, L"Data\\xSpark_2.bmp", 70, 70))
		return false;
	if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_03, L"Data\\xSpark_3.bmp", 70, 70))
		return false;
	if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_04, L"Data\\xSpark_4.bmp", 70, 70))
		return false;

	if (!g_SpriteDib.LoadDibSprite(eGUAGE_HP, L"Data\\HPGuage.bmp", 0, 0))
		return false;

	if (!g_SpriteDib.LoadDibSprite(eSHADOW, L"Data\\Shadow.bmp", 32, 4))
		return false;

	return true;
}