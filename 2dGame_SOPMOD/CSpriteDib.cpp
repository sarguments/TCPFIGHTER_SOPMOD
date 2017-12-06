#include "stdafx.h"
#include "hoxy_Header.h"

#include "CSpriteDib.h"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
	: _maxSprite(iMaxSprite), _colorKey(dwColorKey)
{
	// _colorKey : 투명 색
	// 최대 읽어올 개수 만큼 미리 할당 받는다.
	_sprite = (st_SPRITE*)malloc(sizeof(st_SPRITE) * _maxSprite);

	// 0x00으로 초기화
	memset(_sprite, 0x00, sizeof(sizeof(st_SPRITE) * _maxSprite));
}

CSpriteDib::~CSpriteDib()
{
	int iCount;
	//-----------------------------------------------------------------
	// 전체를 돌면서 모두 지우자.
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
		OutputDebugString(L"파일 열기 에러");
		return FALSE;
	}

	BITMAPFILEHEADER bfh;
	fread_s(&bfh, sizeof(bfh), sizeof(bfh), 1, fp);
	if (bfh.bfType != 0x4d42)
	{
		OutputDebugString(L"비트맵 아님");
		return FALSE;
	}

	BITMAPINFOHEADER bih;
	fread_s(&bih, sizeof(bih), sizeof(bih), 1, fp);

	int pitch = bih.biWidth * (bih.biBitCount / 8);

	// 스프라이트 구조체에 크기 저장
	_sprite[iSpriteIndex].width = bih.biWidth;
	_sprite[iSpriteIndex].height = bih.biHeight;
	_sprite[iSpriteIndex].pitch = pitch;
	_sprite[iSpriteIndex].centerPointX = iCenterPointX;
	_sprite[iSpriteIndex].centerPointY = iCenterPointY;

	// 이미지에 대한 전체 크기 구하고, 메모리할당
	int fileByteSize = pitch * bih.biHeight;
	BYTE* buffer = (BYTE*)malloc(fileByteSize);
	BYTE* reverseBuf = (BYTE*)malloc(fileByteSize);

	// 동적 할당 포인터 저장
	_sprite[iSpriteIndex].bypImage = reverseBuf;

	// 이미지 부분은 스프라이트 버퍼로 읽어온다
	fread_s(buffer, fileByteSize, fileByteSize, 1, fp);
	fclose(fp);

	// DIB는 뒤집어져 있으므로 이를 다시 뒤집자
	// 임시 버퍼에 읽은 뒤에 뒤집으면서 복사한다.
	BYTE* backupP = buffer;
	int scrDibPitch = pitch;

	// 끝으로 이동, 피치 * 높이 - 1 memcpy
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
	// 최대 스프라이트 갯수를 초과하거나, 로드되지 않는 스프라이트라면 무시
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// 지역변수로 필요정보 셋팅
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

	// 화면에 찍을 위치로 이동.. iMAGE + (X * 4) + (Y * Pitch)
	// 출력 중점으로 좌표 계산
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;

	// 새로 할당해서 memcpy 한다음 복사된 메모리 기준으로 출력
	// 원본을 건드리는게 아니라 원본에서 복사받은 로컬변수를 참조해서 처리
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// 상단클리핑.. 스프라이트의 사이즈를 줄이는 느낌(돌만큼만 돈다), 높이줄이고(로컬변수 참조) DEST, SPRITE 이동
	// 스프라이트의 사이즈를 줄이는 느낌, 빠져나간 부분은 반복문을 돌 필요도 없다.
	if (movedCenterY < 0)
	{
		// 출력을 시작할 위치를 위로 벗어난 만큼 내린다.
		toDest -= (movedCenterY * iDestPitch);

		// 출력할 스프라이트도 같이 내린다.
		pMySrcImage -= (movedCenterY * pitch);

		// 벗어난 만큼 높이를 줄인다.
		spriteHeight += movedCenterY;

		// 출력 시작 높이 0으로 변경
		movedCenterY = 0;
	}

	// 하단클리핑
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// 좌측클리핑
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		// 출력 시작 높이 0으로 변경
		movedCenterX = 0;
	}

	// 우측클리핑
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
			// 전체크기 돌면서 픽셀마다 투명색 처리를 하여 그림 출력
			// 칼라키는 알파값 날리고 검사
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

	pMySrcImage = bkPMySrcImg; // 포인터 시작 위치 복구
	free(pMySrcImage);
}

void CSpriteDib::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// 최대 스프라이트 갯수를 초과하거나, 로드되지 않는 스프라이트라면 무시?
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// 지역변수로 필요정보 셋팅
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

	// 화면에 찍을 위치로 이동.. iMAGE + (X * 4) + (Y * Pitch)
	// 출력 중점으로 좌표 계산
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;
	// 새로 할당해서 memcpy 한다음 복사된 메모리 기준으로 출력
	// 원본을 건드리는게 아니라 원본에서 복사받은 로컬변수를 참조해서 처리
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// 상단클리핑.. 스프라이트의 사이즈를 줄이는 느낌(돌만큼만 돈다), 높이줄이고(로컬변수 참조) DEST, SPRITE 이동
	// 스프라이트의 사이즈를 줄이는 느낌, 빠져나간 부분은 반복문을 돌 필요도 없다.
	if (movedCenterY < 0)
	{
		// 높이를 변경
		toDest -= (movedCenterY * iDestPitch);

		//dest,  sprite 옮긴다.
		pMySrcImage -= (movedCenterY * pitch);
		spriteHeight += movedCenterY;

		// 출력 시작 높이 0으로 변경
		movedCenterY = 0;
	}

	// 하단클리핑
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// 좌측클리핑
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		movedCenterX = 0;
	}

	// 우측클리핑
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
			// 전체크기 돌면서 픽셀마다 투명색 처리를 하여 그림 출력
			// 칼라키는 알파값 날리고 검사
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

			//// 반투명
			//*pMySrcImage = (byBlue + *toDest) / 2;
			//*(pMySrcImage + 1) = (byGreen + *(toDest + 1)) / 2;
			//*(pMySrcImage + 2) = (byRed + *(toDest + 2)) / 2;

			//*(DWORD*)toDest = *(DWORD*)pMySrcImage;

			*((DWORD*)toDest) = ((*(DWORD*)pMySrcImage >> 1) & 0x007f7f7f) + ((*(DWORD*)toDest >> 1) & 0x007f7f7f);
			//*(DWORD*)toDest = (((*(DWORD*)pMySrcImage) + (*(DWORD*)toDest)) >> 1) & 0x007f7f7f; // 안됨

			toDest += 4;
			pMySrcImage += 4;
		}

		pMySrcImage = lineFirst + pitch;
		lineFirst = pMySrcImage;

		toDest = destFirst + iDestPitch;
		destFirst = toDest;
	}

	pMySrcImage = bkPMySrcImg; // 포인터 시작 위치 복구
	free(pMySrcImage);
}

void CSpriteDib::DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// 최대 스프라이트 갯수를 초과하거나, 로드되지 않는 스프라이트라면 무시?
	if (_maxSprite > eSPRITE_MAX || bypDest == nullptr)
	{
		return;
	}

	// 지역변수로 필요정보 셋팅
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

	// 화면에 찍을 위치로 이동.. iMAGE + (X * 4) + (Y * Pitch)
	// 출력 중점으로 좌표 계산
	BYTE* toDest = bypDest + ((iDrawX - centerX) * 4) + ((iDrawY - centerY) * iDestPitch);

	int toAllocSize = pitch * spriteHeight;
	// 새로 할당해서 memcpy 한다음 복사된 메모리 기준으로 출력
	// 원본을 건드리는게 아니라 원본에서 복사받은 로컬변수를 참조해서 처리
	BYTE* pMySrcImage = (BYTE*)malloc(toAllocSize);
	BYTE* bkPMySrcImg = pMySrcImage;

	memcpy_s(pMySrcImage, toAllocSize, _sprite[iSpriteIndex].bypImage, toAllocSize);

	// 상단클리핑.. 스프라이트의 사이즈를 줄이는 느낌(돌만큼만 돈다), 높이줄이고(로컬변수 참조) DEST, SPRITE 이동
	// 스프라이트의 사이즈를 줄이는 느낌, 빠져나간 부분은 반복문을 돌 필요도 없다.
	if (movedCenterY < 0)
	{
		// 높이를 변경
		toDest -= (movedCenterY * iDestPitch);

		//dest,  sprite 옮긴다.
		pMySrcImage -= (movedCenterY * pitch);
		spriteHeight += movedCenterY;

		movedCenterY = 0;
	}

	// 하단클리핑
	if ((movedCenterY + spriteHeight) > 480)
	{
		spriteHeight = spriteHeight + (480 - (movedCenterY + spriteHeight));
	}

	// 좌측클리핑
	if (movedCenterX < 0)
	{
		toDest -= (movedCenterX * 4);
		pMySrcImage -= (movedCenterX * 4);
		spriteWidth += movedCenterX;

		// 출력 시작 높이 0으로 변경
		movedCenterX = 0;
	}

	// 우측클리핑
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
			// 전체크기 돌면서 픽셀마다 투명색 처리를 하여 그림 출력
			// 칼라키는 알파값 날리고 검사
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
			//*(DWORD*)toDest = (((*(DWORD*)pMySrcImage) + (*(DWORD*)toDest)) >> 1) & 0x007f7f7f; // 안됨

			toDest += 4;
			pMySrcImage += 4;
		}

		pMySrcImage = lineFirst + pitch;
		lineFirst = pMySrcImage;

		toDest = destFirst + iDestPitch;
		destFirst = toDest;
	}

	pMySrcImage = bkPMySrcImg; // 포인터 시작 위치 복구
	free(pMySrcImage);
}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	// iDestWidth, iDrawLen 안씀, 중점처리 X
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