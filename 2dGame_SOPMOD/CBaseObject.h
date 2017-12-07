#pragma once
class CBaseObject
{
public:
	CBaseObject();
	virtual ~CBaseObject();

	virtual bool Action(void) = 0;
	virtual bool Draw(void) = 0;

	void ActionInput(DWORD dwAction);
	int GetCurX(void);
	int GetCurY(void);
	int GetObjectID(void);
	int GetObjectType(void);
	int GetOldX(void);
	int GetOldY(void);
	int GetSprite(void);
	bool isEndFrame(void);

	void SetPosition(int x, int y);
	void SetObjectID(int param);
	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);

protected:
	void NextFrame(void);
	void SetCurPosition(int x, int y);
	void SetOldPosition(int x, int y);
	void SetObjectType(int param);

protected:
	bool _bEndFrame;
	DWORD _dwActionInput;
	int _iCurX;
	int _iCurY;
	int _iDelayCount;
	int _iFrameDelay;
	int _iObjectID;
	int _iObjectType;
	int _iOldX;
	int _iOldY;
	bool _bSpriteEnd;
	int _iSpriteNow;
	int _iSpriteStart;
	int _iSpriteMax;
};
