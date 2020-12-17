#include "P.h"
#include "Brick.h"
#include "Point.h"

ItemP::ItemP(float x, float y)
{
	oldY = y;
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(21));
	tag = EntityType::P;
	timeDelay = 0;
	alpha = 0;
	isDone = false;
}

void ItemP::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isBroken)
	{
		left = 0;
		top = 0;
		right = left;
		bottom = top;
	}
	if (!isCollis)
	{
		left = x + 10;
		top = y + 10;
		right = left + 2;
		bottom = top + 5;
	}
	else
	{
		if (!isDone)
		{
			left = x;
			top = y;
			right = x + P_BBOX_WIDTH;
			bottom = y + P_BBOX_HEIGHT;
		}
		else
		{
			left = x;
			top = y;
			right = x;
			bottom = y;
		}
	}
}

void ItemP::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	if (isDone)
	{
		return;
	}
	if (make100)
	{
		Point* point = new Point();
		point->SetPosition(x, y);
		point->SetState(MAKE_100);
		make100 = false;
		listEffect.push_back(point);
		isDone = true;
	}
	Entity::Update(dt, coObjects);
	if (isCollis == true && isStart == false && isOnTop == false)
	{
		timeDelay += dt;
		if (timeDelay >= 250)
		{
			alpha = 255;
			vy = -0.09;
			if (oldY - 16 >= y)
			{
				isOnTop = true;
				isCollis = false;
				isStart = true;
			}
		}
	}
	else
	{
		vy = 0;
	}


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();


	CalcPotentialCollisions(coObjects, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			//if (e->obj->GetType() == EntityType::CBRICK)
			//{
			//	x += min_tx * dx + nx * 0.4f;
			//	y += min_ty * dy + ny * 0.4f;

			//	//if (nx != 0) vx = 0;
			//	if (e->ny != 0) vy = 0;

			//	if (e->nx != 0)
			//		vx = -1 * vx;
			//}
		}
	}
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Update(dt, coObjects);
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void ItemP::Render()
{
	if (isBroken)
		return;
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	if (isDone)
	{
		animationSet->at(1)->Render(nx, x, y);
	}
	else
	{
		animationSet->at(0)->Render(nx, x, y, alpha);
	}
	RenderBoundingBox();
}

void ItemP::SetState(int state)
{
}

