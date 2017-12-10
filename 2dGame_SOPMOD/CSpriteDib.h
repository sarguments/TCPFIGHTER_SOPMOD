#pragma once

class CSpriteDib
{
public:
	struct st_SPRITE
	{
		BYTE* bypImage = nullptr;
		int width = 0;
		int height = 0;
		int pitch = 0;

		int centerPointX = 0;
		int centerPointY = 0;
	};

	CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	BOOL LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY);

	void ReleaseSprite(int iSpriteIndex);

	// 특정 메모리 위치에 스프라이트 출력,  클리핑, 칼라키
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, // iDrawX 는 중점
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	// 특정 메모리 위치에 이미지 출력(memset), 클리핑
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:
	// Sprite 배열 정보
	int _maxSprite;
	st_SPRITE* _sprite;

	// 투명 색
	DWORD _colorKey;
};
