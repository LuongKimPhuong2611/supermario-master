#pragma once
#include "Entity.h"
#include "EffectBrick.h"

#define STATE_DESTROYED			0
#define STATE_BRICK_NORMAL		1
#define STATE_COIN_ROTATE		2
#define STATE_COIN_NO_ROTATE	3
#define STATE_HIDE				4
#define	BROKENBRICK_STATE_COLLISION	10
#define TIME_DELAY_MAX 1500


class BrokenBrick : public Entity
{
public:
	bool isColliBrokenbrick;
	vector<LPGAMEENTITY> listPiece;
	ULONGLONG timeTranformation;
	bool vanish;
	bool hasTranformation;
	bool isDestroyed;
	bool tranformation;
	bool isCoin;
	bool isHide;
	DWORD timeDelay;
	DWORD timeDelayMax;
	/*CBrokenBrickPiece* topLeftPiece;
	CBrokenBrickPiece* topRightPiece;
	CBrokenBrickPiece* bottomLeftPiece;
	CBrokenBrickPiece* bottomRightPiece;*/

	BrokenBrick(int id_state);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
};


