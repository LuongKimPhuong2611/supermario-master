#pragma once
#include "Entity.h"
class EffectBrick : public Entity
{

public:
	EffectBrick(D3DXVECTOR2 position, int nx, int deflectFactorY = 1);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};

