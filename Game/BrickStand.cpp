#include "BrickStand.h"

void BrickStand::Render()
{
	//animationSet->at(0)->Render(nx, x, y);
	RenderBoundingBox();
}

void BrickStand::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + frameW;
	b = y + frameH;
}

BrickStand::BrickStand(float frameW, float frameH)
{
	tag = EntityType::BRICKSTAND;
	this->frameH = frameH;
	this->frameW = frameW;
}
