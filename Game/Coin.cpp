#include "Coin.h"

void Coin::Render()
{
	if (!isdone)
	{
		if (isActive)
			return;
		animationSet->at(0)->Render(nx, x, y);
		RenderBoundingBox();
		isdone = false;
	}
}

void Coin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (!isdone)
	{
		l = x;
		t = y;
		r = x + frameW;
		b = y + frameH;
	}
}

Coin::Coin(float frameW, float frameH)
{
	tag = EntityType::COIN;
	//this->frameH = frameH;
	//this->frameW = frameW;
	this->frameH = COIN_BBOX_HEIGHT;
	this->frameW = COIN_BBOX_WIDTH;
}

void Coin::SetDone(bool isDone)
{
	this->isdone = isDone;
}
