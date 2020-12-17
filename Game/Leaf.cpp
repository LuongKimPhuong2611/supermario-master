#include "Leaf.h"
#include "Brick.h"
#include "CBrick.h"
#include "Point.h"
Leaf::Leaf(float posX, float posY)
{
	//alpha = 0;
	x = posX;
	y = posY;
	oldY = y+1;
	oldX = x;
	tag = EntityType::LEAF;
	SetState(0);
	//SetState(MUSHROOM_STATE_WALKING);
	timeDelay = 0;
}

void Leaf::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (!isOnTop)
	{
		left = x + 10;
		top = y + 10;
		right = left + 2;
		bottom = top + 5;
	}
	else
	{
		left = x;
		top = y;
		if (!isDeath)
		{
			right = x + LEAF_BBOX_WIDTH;
			bottom = y + LEAF_BBOX_HEIGHT;
		}
	}
}

void Leaf::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	if (make100)
	{
		Point* point = new Point();
		point->SetPosition(x, y);
		point->SetState(MAKE_100);
		make100 = false;
		listEffect.push_back(point);
		isDeath = true;
	}
	/*if (isDeath)
		return;*/
	Entity::Update(dt, coObjects);
	if (isOnTop)
	{
		if (vy < 0)
			vy = 0;
		if (vx == 0)
		{
			vy = 0.026;
			vx = LEAF_WALKING_SPEED;
		}
		else
		{
			if (abs(x - oldX) > 40)
			{
				vy = 0.026;
				vx = -vx;
				oldX = x;
			}
			else
				vy -= 0.0002;
		}
	}
	if (isCollis == true && isStart == false)
	{
		timeDelay += dt;
		if (timeDelay >= 200)
		{
			vy = -0.04;
			alpha = 255;
			if (oldY - 30 >= y)
			{
				isOnTop = true;
				isCollis = false;
				isStart = true;
			}
		}
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();


	CalcPotentialCollisions(coObjects, coEvents);
	x += dx;
	y += dy;
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Update(dt, coObjects);
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void Leaf::Render()
{
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	if (isDeath)
		return;
	int ani = LEAF_ANI_WALKING;
	if (vx > 0)
		nx = -1;
	else
		nx = 1;
	animationSet->at(ani)->Render(nx, x, y, alpha);
	RenderBoundingBox();
}

void Leaf::SetState(int state)
{
	Entity::SetState(state);
	switch (state)
	{
	case LEAF_STATE_WALKING:
		y -= 2;
		isCollis = true;
		break;
	case 0:
		alpha = 0;
		vx = 0;
		vy = 0;
		isCollis = false;
		break;
	}
}

