#pragma once
#include "Entity.h"

#define P_WALKING_SPEED 0.04f;

#define P_BBOX_WIDTH		16
#define P_BBOX_HEIGHT	16



class ItemP : public Entity
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();

public:
	DWORD timeDelay;
	vector <LPGAMEENTITY> listEffect;
	int alpha;
	float oldY;
	bool isDone;
	bool isCollis;
	bool isStart = false;
	bool isOnTop = false;
	bool isBroken = false;
	ItemP(float x, float y);
	virtual void SetState(int state);
};

