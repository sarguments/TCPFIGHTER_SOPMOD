#include "stdafx.h"
#include "hoxy_Header.h"

#include "CSpriteDib.h"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
	: _maxSprite(iMaxSprite), _colorKey(dwColorKey)
{
	// _colorKey : ���� ��
	// �ִ� �о�� ���� ��ŭ �̸� �Ҵ� �޴´�.
	_sprite = (st_SPRITE*)malloc(sizeof(st_SPRITE) * _maxSprite);

	// 0x00���� �ʱ�ȭ
	memset(_sprite, 0x00, sizeof(sizeof(st_SPRITE) * _maxSprite));
}

CSpriteDib::~CSpriteDib()
{
	int iCount;
	//-----------------------------------------------------------------
	// ��ü�� ���鼭 ��� ������.
	//-----------------------------------------------------------------
	for (iCount = 0; iCount > _maxSprite; iCount++)
	{
		ReleaseSprite(iCount);
	}
}

BOOL CSpriteDib::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	FILE* fp;
	errno_t err = _wfopen_s(&fp, szFileName, L"rb");
	if (err != 0)
	{
		OutputDebugString(L"���� ���� ����");
		return FALSE;
	}

	BITMAPFILEHEADER bfh;
	fread_s(&bfh, sizeof(bfh), sizeof(bfh), 1, fp);
	if (bfh.bfType != 0x4d42)
	{
		OutputDebugString(L"��Ʈ�� �ƴ�");
		return FALSE;
	}

	BITMAPINFOHEADER bih;
	fread_s(&bih, sizeof(bih), sizeof(bih), 1, fp);

	int pitch = bih.biWidth * (bih.biBitCount / 8);

	// ��������Ʈ ����ü�� ũ�� ����
	_sprite[iSpriteIndex].width = bih.biWidth;
	_sprite[iSpriteIndex].height = bih.biHeight;
	_sprite[iSpriteIndex].pitch = pitch;
	_sprite[iSpriteIndex].centerPointX = iCenterPointX;
	_sprite[iSpriteIndex].centerPointY = iCenterPointY;

	// �̹����� ���� ��ü ũ�� ���ϰ�, �޸��Ҵ�
	int fileByteSize = pitch * bih.biHeight;
	BYTE* buffer = (BYTE*)malloc(fileByteSize);
	BYTE* reverseBuf = (BYTE*)malloc(fileByteSize);

	// ���� �Ҵ� ������ ����
	_sprite[iSpriteIndex].bypImage = reverseBuf;

	// �̹��� �κ��� ��������Ʈ ���۷� �о�´�
	fread_s(buffer, fileByteSize, fileByteSize, 1, fp);
	fclose(fp);

	// DIB�� �������� �����Ƿ� �̸� �ٽ� ������
	// �ӽ� ���ۿ� ���� �ڿ� �������鼭 �����Ѵ�.
	BYTE* backupP = buffer;
	int scrDibPitch = pitch;

	// ������ �̵�, ��ġ * ���� - 1 memcpy
	buffer += pitch * (bih.biHeight - 1);

	for (int i = 0; i < bih.biHeight; i++)
	{
		memcpy_s(reverseBuf, bih.biWidth * 4, buffer, bih.biWidth * 4);

		buffer -= pitch;
		reverseBuf += scrDibPitch;
	}

	buffer = backupP;

	free(buffer);

	return TRUE;
}

void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{
	if (_sprite[iSpriteIndex].bypImage == nullptr)
	{
		return;
	}

	free(_sprite[iSpriteIndex].bypImage);

	memset(&_sprite[iSpriteIndex], 0x00, sizeof(st_SPRITE));
}

void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// ���������� �ʿ����� ����
	int centerX = _sprite[iSpriteIndex].centerPointX;
	int centerY = _sprite[iSpriteIndex].centerPointY;

	int movedCenterX = iDrawX - centerX;
	int movedCenterY = iDrawY - centerY;

	int destWidth = iDestWidth;
	int destHeight = iDestHeight;

	int spriteWidth = static_cast<int>(_sprite[iSpriteIndex].width *
		(static_cast<float>(iDrawLen) / 100));
	int spriteHeight = _sprite[iSpriteIndex].height;

	int pitch = _sprite[iSpriteIndex].width * 4;

	// ȭ�鿡 ���� ��ġ�� �̵�.. iMAGE + (X * 4) + (Y * Pitch)
	// ��� �������� ��ǥ ���
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;

	// ���� �Ҵ��ؼ� memcpy �Ѵ��� ����� �޸� �������� ���
	// ������ �ǵ帮�°� �ƴ϶� �������� ������� ���ú����� �����ؼ� ó��
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// ���Ŭ����.. ��������Ʈ�� ����� ���̴� ����(����ŭ�� ����), �������̰�(���ú��� ����) DEST, SPRITE �̵�
	// ��������Ʈ�� ����� ���̴� ����, �������� �κ��� �ݺ����� �� �ʿ䵵 ����.
	if (movedCenterY < 0)
	{
		// ����� ������ ��ġ�� ���� ��� ��ŭ ������.
		toDest -= (movedCenterY * iDestPitch);

		// ����� ��������Ʈ�� ���� ������.
		pMySrcImage -= (movedCenterY * pitch);

		// ��� ��ŭ ���̸� ���δ�.
		spriteHeight += movedCenterY;

		// ��� ���� ���� 0���� ����
		movedCenterY = 0;
	}

	// �ϴ�Ŭ����
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// ����Ŭ����
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		// ��� ���� ���� 0���� ����
		movedCenterX = 0;
	}

	// ����Ŭ����
	if ((movedCenterX + spriteWidth) > dfSCREEN_WIDTH)
	{
		spriteWidth = spriteWidth + (dfSCREEN_WIDTH - (movedCenterX + spriteWidth));
	}

	BYTE* destFirst = toDest;
	BYTE* lineFirst = pMySrcImage;

	for (int i = 0; i < spriteHeight; i++)
	{
		for (int j = 0; j < spriteWidth; j++)
		{
			// ��üũ�� ���鼭 �ȼ����� ����� ó���� �Ͽ� �׸� ���
			// Į��Ű�� ���İ� ������ �˻�
			DWORD temp = *(DWORD*)pMySrcImage & 0x00ffffff;
			if (temp == _colorKey)
			{
				toDest += 4;
				pMySrcImage += 4;
				continue;
			}

			*(DWORD*)toDest = *(DWORD*)pMySrcImage;

			toDest += 4;
			pMySrcImage += 4;
		}

		pMySrcImage = lineFirst + pitch;
		lineFirst = pMySrcImage;

		toDest = destFirst + iDestPitch;
		destFirst = toDest;
	}

	pMySrcImage = bkPMySrcImg; // ������ ���� ��ġ ����
	free(pMySrcImage);
}

void CSpriteDib::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����?
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// ���������� �ʿ����� ����
	int centerX = _sprite[iSpriteIndex].centerPointX;
	int centerY = _sprite[iSpriteIndex].centerPointY;

	int movedCenterX = iDrawX - centerX;
	int movedCenterY = iDrawY - centerY;

	int destWidth = iDestWidth;
	int destHeight = iDestHeight;

	int spriteWidth = static_cast<int>(_sprite[iSpriteIndex].width *
		(static_cast<float>(iDrawLen) / 100));
	int spriteHeight = _sprite[iSpriteIndex].height;

	int pitch = _sprite[iSpriteIndex].width * 4;

	// ȭ�鿡 ���� ��ġ�� �̵�.. iMAGE + (X * 4) + (Y * Pitch)
	// ��� �������� ��ǥ ���
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;
	// ���� �Ҵ��ؼ� memcpy �Ѵ��� ����� �޸� �������� ���
	// ������ �ǵ帮�°� �ƴ϶� �������� ������� ���ú����� �����ؼ� ó��
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// ���Ŭ����.. ��������Ʈ�� ����� ���̴� ����(����ŭ�� ����), �������̰�(���ú��� ����) DEST, SPRITE �̵�
	// ��������Ʈ�� ����� ���̴� ����, �������� �κ��� �ݺ����� �� �ʿ䵵 ����.
	if (movedCenterY < 0)
	{
		// ���̸� ����
		toDest -= (movedCenterY * iDestPitch);

		//dest,  sprite �ű��.
		pMySrcImage -= (movedCenterY * pitch);
		spriteHeight += movedCenterY;

		// ��� ���� ���� 0���� ����
		movedCenterY = 0;
	}

	// �ϴ�Ŭ����
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// ����Ŭ����
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		movedCenterX = 0;
	}

	// ����Ŭ����
	if ((movedCenterX + spriteWidth) > dfSCREEN_WIDTH)
	{
		spriteWidth = spriteWidth + (dfSCREEN_WIDTH - (movedCenterX + spriteWidth));
	}

	BYTE* destFirst = toDest;
	BYTE* lineFirst = pMySrcImage;

	//BYTE byBlue;
	//BYTE byGreen;
	//BYTE byRed;

	for (int i = 0; i < spriteHeight; i++)
	{
		for (int j = 0; j < spriteWidth; j++)
		{
			// ��üũ�� ���鼭 �ȼ����� ����� ó���� �Ͽ� �׸� ���
			// Į��Ű�� ���İ� ������ �˻�
			DWORD temp = *(DWORD*)pMySrcImage & 0x00ffffff;
			if (temp == _colorKey)
			{
				toDest += 4;
				pMySrcImage += 4;
				continue;
			}

			//byBlue = *pMySrcImage;
			//byGreen = *(pMySrcImage + 1);
			//byRed = *(pMySrcImage + 2);

			//// ������
			//*pMySrcImage = (byBlue + *toDest) / 2;
			//*(pMySrcImage + 1) = (byGreen + *(toDest + 1)) / 2;
			//*(pMySrcImage + 2) = (byRed + *(toDest + 2)) / 2;

			//*(DWORD*)toDest = *(DWORD*)pMySrcImage;

			*((DWORD*)toDest) = ((*(DWORD*)pMySrcImage >> 1) & 0x007f7f7f) + ((*(DWORD*)toDest >> 1) & 0x007f7f7f);
			//*(DWORD*)toDest = (((*(DWORD*)pMySrcImage) + (*(DWORD*)toDest)) >> 1) & 0x007f7f7f; // �ȵ�

			toDest += 4;
			pMySrcImage += 4;
		}

		pMySrcImage = lineFirst + pitch;
		lineFirst = pMySrcImage;

		toDest = destFirst + iDestPitch;
		destFirst = toDest;
	}

	pMySrcImage = bkPMySrcImg; // ������ ���� ��ġ ����
	free(pMySrcImage);
}

void CSpriteDib::DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// �ִ� ��������Ʈ ������ �ʰ��ϰų�, �ε���� �ʴ� ��������Ʈ��� ����?
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// ���������� �ʿ����� ����
	int centerX = _sprite[iSpriteIndex].centerPointX;
	int centerY = _sprite[iSpriteIndex].centerPointY;

	int movedCenterX = iDrawX - centerX;
	int movedCenterY = iDrawY - centerY;

	int destWidth = iDestWidth;
	int destHeight = iDestHeight;

	int spriteWidth = static_cast<int>(_sprite[iSpriteIndex].width *
		(static_cast<float>(iDrawLen) / 100));
	int spriteHeight = _sprite[iSpriteIndex].height;

	int pitch = _sprite[iSpriteIndex].width * 4;

	// ȭ�鿡 ���� ��ġ�� �̵�.. iMAGE + (X * 4) + (Y * Pitch)
	// ��� �������� ��ǥ ���
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;
	// ���� �Ҵ��ؼ� memcpy �Ѵ��� ����� �޸� �������� ���
	// ������ �ǵ帮�°� �ƴ϶� �������� ������� ���ú����� �����ؼ� ó��
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// ���Ŭ����.. ��������Ʈ�� ����� ���̴� ����(����ŭ�� ����), �������̰�(���ú��� ����) DEST, SPRITE �̵�
	// ��������Ʈ�� ����� ���̴� ����, �������� �κ��� �ݺ����� �� �ʿ䵵 ����.
	if (movedCenterY < 0)
	{
		// ���̸� ����
		toDest -= (movedCenterY * iDestPitch);

		//dest,  sprite �ű��.
		pMySrcImage -= (movedCenterY * pitch);
		spriteHeight += movedCenterY;

		movedCenterY = 0;
	}

	// �ϴ�Ŭ����
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// ����Ŭ����
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		// ��� ���� ���� 0���� ����
		movedCenterX = 0;
	}

	// ����Ŭ����
	if ((movedCenterX + spriteWidth) > dfSCREEN_WIDTH)
	{
		spriteWidth = spriteWidth + (dfSCREEN_WIDTH - (movedCenterX + spriteWidth));
	}

	BYTE* destFirst = toDest;
	BYTE* lineFirst = pMySrcImage;

	BYTE byBlue;
	BYTE byGreen;
	BYTE byRed;
	BYTE byGray;

	for (int i = 0; i < spriteHeight; i++)
	{
		for (int j = 0; j < spriteWidth; j++)
		{
			// ��üũ�� ���鼭 �ȼ����� ����� ó���� �Ͽ� �׸� ���
			// Į��Ű�� ���İ� ������ �˻�
			DWORD temp = *(DWORD*)pMySrcImage & 0x00ffffff;
			if (temp == _colorKey)
			{
				toDest += 4;
				pMySrcImage += 4;
				continue;
			}

			byBlue = *pMySrcImage;
			byGreen = *(pMySrcImage + 1);
			byRed = *(pMySrcImage + 2);
			byGray = (byBlue + byGreen + byRed) / 3;

			//*pMySrcImage = byGray;
			//*(pMySrcImage + 1) = byGray;
			//*(pMySrcImage + 2) = byGray;

			*pMySrcImage = byBlue / 2;
			*(pMySrcImage + 1) = byGreen / 2;
			*(pMySrcImage + 2) = byGray;

			*(DWORD*)toDest = *(DWORD*)pMySrcImage;

			//*((DWORD*)toDest) = ((*(DWORD*)pMySrcImage >> 1) & 0x007f7f7f) + ((*(DWORD*)toDest >> 1) & 0x007f7f7f);
			//*(DWORD*)toDest = (((*(DWORD*)pMySrcImage) + (*(DWORD*)toDest)) >> 1) & 0x007f7f7f; // �ȵ�

			toDest += 4;
			pMySrcImage += 4;
		}

		pMySrcImage = lineFirst + pitch;
		lineFirst = pMySrcImage;

		toDest = destFirst + iDestPitch;
		destFirst = toDest;
	}

	pMySrcImage = bkPMySrcImg; // ������ ���� ��ġ ����
	free(pMySrcImage);
}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// iDestWidth, iDrawLen �Ⱦ�, ����ó�� X
	BYTE* pSrcImage = _sprite[iSpriteIndex].bypImage;
	BYTE* dest = bypDest + (iDrawX * 4) + (iDrawY * iDestPitch);

	int toCpySize = _sprite[iSpriteIndex].pitch;

	for (int i = 0; i < iDestHeight; i++)
	{
		memcpy_s(dest, toCpySize, pSrcImage, toCpySize);

		pSrcImage += _sprite[iSpriteIndex].pitch;
		dest += iDestPitch;
	}
}