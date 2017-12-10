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

	// Ư�� �޸� ��ġ�� ��������Ʈ ���,  Ŭ����, Į��Ű
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, // iDrawX �� ����
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	void DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	// Ư�� �޸� ��ġ�� �̹��� ���(memset), Ŭ����
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:
	// Sprite �迭 ����
	int _maxSprite;
	st_SPRITE* _sprite;

	// ���� ��
	DWORD _colorKey;
};
