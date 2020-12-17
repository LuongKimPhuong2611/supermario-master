#include "BrokenBrick.h"
BrokenBrick::BrokenBrick(int id_state)
{
	tag = EntityType::BROKENBRICK;
	id_broken_state = id_state;
	if (this->id_broken_state == 1)
		SetState(STATE_BRICK_NORMAL);
	/*else if (this->id_brick_items == 2)
		SetState(STATE_COIN_NO_ROTATE);*/
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(20));
	isCoin = false;
	timeDelay = 0;
	timeDelayMax = TIME_DELAY_MAX;
}

void BrokenBrick::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	if (isHide)
		return;
	if (isCoin)
	{
		this->SetAnimationSet(CAnimationSets::GetInstance()->Get(20));
		timeDelay += dt;
		if (timeDelay >= timeDelayMax)
		{
			isCoin = false;
			timeDelay = 0;
		}
	}
	//DebugOut(L"Gia tri cua state %d \n", id_broken_state);
	/*if (isDestroyed)
		DebugOut(L"dasdasdasdasda");*/
	/*if (isDestroyed)
		return;*/
	for (int i = 0; i < listPiece.size(); i++)
	{
		listPiece[i]->Update(dt, coObjects);
	}
}

void BrokenBrick::Render()
{
	if (isHide)
		return;
	/*if (isColliBrokenbrick)
	{
		animationSet->at(3)->Render(nx, x, y);
	}*/
	/*if (isDestroyed)
		return;*/
	RenderBoundingBox();
	if (isCoin)
	{
		this->SetAnimationSet(CAnimationSets::GetInstance()->Get(16));
		timeDelay += dt;
		if (timeDelay >= timeDelayMax)
		{
			isCoin = false;
			this->SetAnimationSet(CAnimationSets::GetInstance()->Get(20));
			timeDelay = 0;
			animationSet->at(0)->Render(nx, x, y);
		}
	}
	else
	{
		if (!isDestroyed)
			animationSet->at(0)->Render(nx, x, y);
		//DebugOut(L"Gia tri cua state %d \n", state);
		for (LPGAMEENTITY piece : listPiece)
		{
			piece->Render();
		}
	}
}
void BrokenBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isHide)
		return;
	if (isDestroyed)
		return;
	l = x;
	t = y;
	r = x + 16;
	b = y + 16;
}
void BrokenBrick::SetState(int State)
{
	Entity::SetState(State);
	switch (State)
	{
	case BROKENBRICK_STATE_COLLISION:
		if (!isColliBrokenbrick)
		{
			y -= 16;
			vy += 0.05;
			isColliBrokenbrick = true;
		}
		break;
	case STATE_HIDE:
		isHide = true;
		break;
	case STATE_BRICK_NORMAL:
		isHide = false;
		break;
	case STATE_COIN_NO_ROTATE:
		break;
	case STATE_COIN_ROTATE:
		break;
	case STATE_DESTROYED:
		isDestroyed = true;
		EffectBrick* topLeftPiece = new EffectBrick({ x - 1, y - 2 }, -1, 2);
		EffectBrick* topRightPiece = new EffectBrick({ x + 9, y - 2 }, 1, 2);
		EffectBrick* bottomLeftPiece = new EffectBrick({ x - 1, y + 8 }, -1);
		EffectBrick* bottomRightPiece = new EffectBrick({ x + 9, y + 8 }, 1);
		listPiece = { topLeftPiece, topRightPiece, bottomLeftPiece, bottomRightPiece };
		break;
	}

	/*if (State == STATE_BRICK_NORMAL)
	{

	}
	else if (State == STATE_COIN_NO_ROTATE)
	{

	}*/

}
