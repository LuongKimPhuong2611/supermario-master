#include "Koopa.h"
#include "Point.h"
#include "BrokenBrick.h"
#include "Brick.h"
#include "CBrick.h"

Koopa::Koopa(Player* mario, int id_Koopa)
{
	tag = EntityType::KOOPA;
	Mario = mario;
	id_koopa = id_Koopa;
	if (id_koopa == KOOPA_RED)
		SetState(KOOPA_RED_STATE_WALKING_RIGHT);
	else if (id_koopa == KOOPA_GREEN)
	{
		SetState(KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT);
		timeToFly = GetTickCount64();

	}
}

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x + 1;
	top = y + 11;
	bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
	right = x + KOOPAS_BBOX_WIDTH;
	if (id_koopa == KOOPA_RED)
	{
		/*if (state == KOOPA_RED_STATE_DIE)
		{

			bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
		}
		else if (state == KOOPA_RED_STATE_DIE_AND_MOVE || state == KOOPA_RED_STATE_DIE_AND_MOVE_UP)
		{
			bottom = top + KOOP	AS_BBOX_HEIGHT_DIE;
		}
		else*/
		if (state == KOOPA_RED_STATE_REVIVE || state == KOOPA_RED_STATE_REVIVE_UP)
		{
			left = x;
			bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
		}
		else if (state == KOOPA_RED_STATE_WALKING_RIGHT || state == KOOPA_RED_STATE_WALKING_LEFT)
		{
			top = y;
			bottom = top + KOOPAS_BBOX_HEIGHT;
		}
		else if (state == KOOPA_RED_STATE_DIE_UP)
		{
			/*if (!hitByTail)
			{
				left = right;
				top = bottom;
			}*/
			if (hitByWeapon)
				left = top = right = bottom = 0;
			//bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
		}
		/*else if (state == KOOPA_RED_STATE_HOLDING || state == KOOPA_RED_STATE_HOLDING_UP)
		{
			bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
		}*/
	}
	else if (id_koopa == KOOPA_GREEN)
	{
		if (state == KOOPA_GREEN_STATE_REVIVE || state == KOOPA_GREEN_STATE_REVIVE_UP)
		{
			left = x;
			bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
		}
		else if (state == KOOPA_GREEN_STATE_WALKING_RIGHT || state == KOOPA_GREEN_STATE_WALKING_LEFT || state == KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT || state == KOOPA_GREEN_STATE_HAS_WING_FLY_RIGHT)
		{
			top = y;
			bottom = top + KOOPAS_BBOX_HEIGHT;
		}
		else if (state == KOOPA_GREEN_STATE_DIE_UP)
		{
			if (hitByWeapon)
				left = top = right = bottom = 0;
		}

	}
}

void Koopa::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{

	if (GetState() == KOOPA_RED_STATE_HOLDING ||
		GetState() == KOOPA_RED_STATE_HOLDING_UP ||
		GetState() == KOOPA_GREEN_STATE_HOLDING ||
		GetState() == KOOPA_GREEN_STATE_HOLDING_UP)
	{
		if (Mario->level == MARIO_LEVEL_RACCOON)
		{
			if (Mario->nx > 0)
				SetPosition(Mario->x + 10, Mario->y - 5);
			else
				SetPosition(Mario->x - 14, Mario->y - 5);
		}
		else if (Mario->level == MARIO_LEVEL_SMALL)
		{
			if (Mario->nx > 0)
				SetPosition(Mario->x + 10, Mario->y - 15);
			else
				SetPosition(Mario->x - 13, Mario->y - 15);
		}
		else
		{
			if (Mario->nx > 0)
				SetPosition(Mario->x + 11, Mario->y - 5);
			else
				SetPosition(Mario->x - 13, Mario->y - 5);
		}
		//state = KOOPAS_ANI_DIE;
	}
	if (id_koopa == KOOPA_RED)
	{
		if (!Mario->holdthing && (last_state == KOOPA_RED_STATE_HOLDING || last_state == KOOPA_RED_STATE_HOLDING_UP))//de khi tha mai rua ra thi mai rua bi da
		{
			nx = Mario->nx;
			if (last_state == KOOPA_RED_STATE_HOLDING)
				SetState(KOOPA_RED_STATE_DIE_AND_MOVE);
			else if (last_state == KOOPA_RED_STATE_HOLDING_UP)
				SetState(KOOPA_RED_STATE_DIE_AND_MOVE_UP);

		}
		if (GetTickCount64() - timeToRevive > 8000 && (last_state == KOOPA_RED_STATE_DIE || last_state == KOOPA_RED_STATE_DIE_UP || last_state == KOOPA_RED_STATE_HOLDING || last_state == KOOPA_RED_STATE_HOLDING_UP))//koopas vao trang thai chuan bi hoi sinh
		{
			Mario->holdthing = false;
			if (last_state == KOOPA_RED_STATE_DIE || last_state == KOOPA_RED_STATE_HOLDING)
				SetState(KOOPA_RED_STATE_REVIVE);
			else if (last_state == KOOPA_RED_STATE_DIE_UP || last_state == KOOPA_RED_STATE_HOLDING_UP)
				SetState(KOOPA_RED_STATE_REVIVE_UP);
			hitByTail = false;
		}
		if (GetTickCount64() - timeToRevive > 10000 && (last_state == KOOPA_RED_STATE_REVIVE || last_state == KOOPA_RED_STATE_REVIVE_UP))
		{
			SetState(KOOPA_RED_STATE_WALKING_RIGHT);
			timeToRevive = 0;
			Mario->holdthing = false;
		}
	}
	else if (id_koopa == KOOPA_GREEN)
	{
		if (hasWing)
		{
			if (GetTickCount64() - timeToFly > 900 && !hitByWeapon)
			{
				SetState(KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT);
				timeToFly = GetTickCount64();
			}
		}
		else
		{
			if (!Mario->holdthing && (last_state == KOOPA_GREEN_STATE_HOLDING || last_state == KOOPA_GREEN_STATE_HOLDING_UP))//de khi tha mai rua ra thi mai rua bi da
			{
				nx = Mario->nx;
				if (last_state == KOOPA_GREEN_STATE_HOLDING)
					SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE);
				else if (last_state == KOOPA_GREEN_STATE_HOLDING_UP)
					SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE_UP);
				hitByTail = false;
			}
			if (GetTickCount64() - timeToRevive > 8000 && (last_state == KOOPA_GREEN_STATE_DIE || last_state == KOOPA_GREEN_STATE_DIE_UP || last_state == KOOPA_GREEN_STATE_HOLDING || last_state == KOOPA_GREEN_STATE_HOLDING_UP))//koopas vao trang thai chuan bi hoi sinh
			{
				Mario->holdthing = false;
				if (last_state == KOOPA_GREEN_STATE_DIE || last_state == KOOPA_GREEN_STATE_HOLDING)
					SetState(KOOPA_GREEN_STATE_REVIVE);
				else if (last_state == KOOPA_GREEN_STATE_DIE_UP || last_state == KOOPA_GREEN_STATE_HOLDING_UP)
					SetState(KOOPA_GREEN_STATE_REVIVE_UP);
			}
			if (GetTickCount64() - timeToRevive > 10000 && (last_state == KOOPA_GREEN_STATE_REVIVE || last_state == KOOPA_GREEN_STATE_REVIVE_UP))
			{
				SetState(KOOPA_GREEN_STATE_WALKING_RIGHT);
				timeToRevive = 0;
			}
		}
	}

	if (make100)
	{
		Point* point = new Point();
		point->SetPosition(x, y);
		point->SetState(MAKE_100);
		make100 = false;
		listEffect.push_back(point);
	}

	Entity::Update(dt);
	if (state != KOOPA_RED_STATE_HOLDING &&
		state != KOOPA_RED_STATE_HOLDING_UP &&
		state != KOOPA_GREEN_STATE_HOLDING &&
		state != KOOPA_GREEN_STATE_HOLDING_UP)
		vy += 0.0005f * dt;
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{

		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;
		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block every object first!

		//x += min_tx * dx + nx * 0.5f;
		y += min_ty * dy + ny * 0.1f;

		if (ny != 0)
			vy = 0;
		if (ny != 0 && (state == KOOPA_RED_STATE_DIE_UP || state == KOOPA_RED_STATE_REVIVE_UP || state == KOOPA_GREEN_STATE_REVIVE_UP || state == KOOPA_GREEN_STATE_DIE_UP))
		{
			vx = 0;
		}

		//Collision logic with other objects

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (id_koopa == KOOPA_RED)
			{
				/*if (dynamic_cast<CBrick*>(e->obj))
				{
					CBrick* cbrick = dynamic_cast<CBrick*>(e->obj);
					if (e->nx != 0)
					{
						x += dx;
					}
				}*/
				if (dynamic_cast<Brick*>(e->obj))
				{
					Brick* brick = dynamic_cast<Brick*>(e->obj);
					if (e->nx != 0)
					{
						x += dx;
					}
					else if (e->ny < 0 && GetState() != KOOPA_RED_STATE_DIE_AND_MOVE && GetState() != KOOPA_RED_STATE_DIE_AND_MOVE_UP)
					{
						if (x <= brick->x)
						{
							x = brick->x;
							SetState(KOOPA_RED_STATE_WALKING_RIGHT);
							vx = KOOPAS_WALKING_SPEED;

						}
						else if (x >= brick->x + brick->frameW - 1 - KOOPAS_BBOX_WIDTH)
						{
							x = brick->x + brick->frameW - 1 - KOOPAS_BBOX_WIDTH;
							SetState(KOOPA_RED_STATE_WALKING_LEFT);
							vx = -KOOPAS_WALKING_SPEED;
						}
					}
					else if (e->nx != 0 && GetState() == KOOPA_RED_STATE_DIE_AND_MOVE && GetState() == KOOPA_RED_STATE_DIE_AND_MOVE_UP) {
						//this->nx = -this->nx;
						//x += dx;
					}
				}
				else if (dynamic_cast<BrokenBrick*>(e->obj))
				{
					if (GetState() == KOOPA_RED_STATE_DIE_AND_MOVE || GetState() == KOOPA_RED_STATE_DIE_AND_MOVE_UP)
					{
						BrokenBrick* brokenbrick = dynamic_cast<BrokenBrick*>(e->obj);
						//brokenbrick->isDestroyed = true;
						brokenbrick->SetState(STATE_DESTROYED);
						vx = -vx;
					}
				}
				else
				{
					if (e->nx != 0)
					{
						vx = -vx;
						if (GetState() != KOOPA_RED_STATE_DIE_AND_MOVE && GetState() != KOOPA_RED_STATE_DIE_AND_MOVE_UP && GetState() != KOOPA_RED_STATE_DIE_UP)
						{
							if (vx > 0)
								SetState(KOOPA_RED_STATE_WALKING_RIGHT);
							else
								SetState(KOOPA_RED_STATE_WALKING_LEFT);

						}
					}
				}
			}
			else if (id_koopa == KOOPA_GREEN)
			{
				if (dynamic_cast<Brick*>(e->obj))
				{
					if (e->nx != 0)
					{
						x += dx;
					}
				}
				else if (dynamic_cast<BrokenBrick*>(e->obj))
				{
					if (GetState() == KOOPA_GREEN_STATE_DIE_AND_MOVE || GetState() == KOOPA_GREEN_STATE_DIE_AND_MOVE_UP)
					{
						BrokenBrick* brokenbrick = dynamic_cast<BrokenBrick*>(e->obj);
						//brokenbrick->isDestroyed = true;
						brokenbrick->SetState(STATE_DESTROYED);
						vx = -vx;
					}
				}
				else
				{
					if (!hasWing)
					{
						if (e->nx != 0)
						{
							vx = -vx;
							if (GetState() != KOOPA_GREEN_STATE_DIE_AND_MOVE && GetState() != KOOPA_GREEN_STATE_DIE_AND_MOVE_UP && GetState() != KOOPA_GREEN_STATE_DIE_UP)
							{
								if (vx > 0)
									SetState(KOOPA_GREEN_STATE_WALKING_RIGHT);
								else
									SetState(KOOPA_GREEN_STATE_WALKING_LEFT);
							}
						}
					}
				}
			}
		}

	}
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Update(dt, coObjects);
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	CheckCollisionWithBrick(coObjects);
}

void Koopa::CheckCollisionWithBrick(vector<LPGAMEENTITY>* coObjects)
{
	float l_brick, t_brick, r_brick, b_brick, l_koopa, t_koopa, r_koopa, b_koopa;
	GetBoundingBox(l_koopa, t_koopa, r_koopa, b_koopa);
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPGAMEENTITY e = coObjects->at(i);
		if (dynamic_cast<CBrick*>(e) && GetState() == KOOPA_GREEN_STATE_DIE_AND_MOVE)
		{
			CBrick* brick = dynamic_cast<CBrick*>(e);
			brick->GetBoundingBox(l_brick, t_brick, r_brick, b_brick);
			if (Entity::CheckAABB(l_brick, t_brick, r_brick, b_brick, l_koopa, t_koopa, r_koopa, b_koopa))
			{
				SetState(KOOPA_GREEN_STATE_DIE_UP);
				hitByWeapon = true;
			}
		}
	}
}

void Koopa::Render()
{
	//DebugOut(L"nx %d ani %d \n", this->nx, state);
	animationSet->at(state)->Render(-this->nx, x, y);
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	//DebugOut(L"gia tri state %d \n", state);
	RenderBoundingBox();
}

void Koopa::SetState(int State)
{
	Entity::SetState(State);   // last_state de xac dinh trang thai truoc do cua koopas roi thuc hien cac truong hop
	switch (State)
	{
	case KOOPA_RED_STATE_DIE:
		vx = 0;
		vy = 0;
		timeToRevive = GetTickCount64();
		last_state = KOOPA_RED_STATE_DIE;
		break;
	case KOOPA_RED_STATE_DIE_UP:
		if (Mario->nx > 0)
			vx = 0.05;
		else
			vx = -0.05;
		if (last_state != KOOPA_RED_STATE_DIE_AND_MOVE_UP)
			vy = -0.15;
		timeToRevive = GetTickCount64();
		last_state = KOOPA_RED_STATE_DIE_UP;
		break;
	case KOOPA_RED_STATE_WALKING_RIGHT:
		/*if (last_state == KOOPA_RED_STATE_REVIVE || last_state == KOOPA_RED_STATE_REVIVE_UP)
			y -= KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE;*/
		vx = KOOPAS_WALKING_SPEED;
		nx = 1;
		last_state = KOOPA_RED_STATE_WALKING_RIGHT;
		break;
	case KOOPA_RED_STATE_WALKING_LEFT:
		vx = -KOOPAS_WALKING_SPEED;
		nx = -1;
		last_state = KOOPA_RED_STATE_WALKING_LEFT;
		break;
	case KOOPA_RED_STATE_DIE_AND_MOVE:
		//vx = nx * 0.1;
		vx = Mario->nx* 0.2;
		last_state = KOOPA_RED_STATE_DIE_AND_MOVE;
		break;
	case KOOPA_RED_STATE_REVIVE:
		last_state = KOOPA_RED_STATE_REVIVE;
		break;
	case KOOPA_RED_STATE_REVIVE_UP:
		last_state = KOOPA_RED_STATE_REVIVE_UP;
		break;
	case KOOPA_RED_STATE_DIE_AND_MOVE_UP:
		last_state = KOOPA_RED_STATE_DIE_AND_MOVE_UP;
		vx = 0.1 * nx;
		break;
	case KOOPA_RED_STATE_HOLDING:
		last_state = KOOPA_RED_STATE_HOLDING;
		break;
	case KOOPA_RED_STATE_HOLDING_UP:
		last_state = KOOPA_RED_STATE_HOLDING_UP;
		break;
	case KOOPA_GREEN_STATE_WALKING_RIGHT:
		vx = KOOPAS_WALKING_SPEED;
		nx = 1;
		last_state = KOOPA_GREEN_STATE_WALKING_RIGHT;
		break;
	case KOOPA_GREEN_STATE_WALKING_LEFT:
		vx = -KOOPAS_WALKING_SPEED;
		nx = -1;
		last_state = KOOPA_GREEN_STATE_WALKING_LEFT;
		break;
	case KOOPA_GREEN_STATE_DIE:
		vx = 0;
		vy = 0;
		timeToRevive = GetTickCount64();
		last_state = KOOPA_GREEN_STATE_DIE;
		break;
	case KOOPA_GREEN_STATE_DIE_AND_MOVE:
		last_state = KOOPA_GREEN_STATE_DIE_AND_MOVE;
		vx = Mario->nx * 0.2;
		break;
	case KOOPA_GREEN_STATE_DIE_UP:
		if (Mario->nx > 0)
			vx = 0.05;
		else
			vx = -0.05;
		if (last_state != KOOPA_GREEN_STATE_DIE_AND_MOVE_UP)
			vy = -0.13;
		timeToRevive = GetTickCount64();
		last_state = KOOPA_GREEN_STATE_DIE_UP;
		break;
	case KOOPA_GREEN_STATE_DIE_AND_MOVE_UP:
		last_state = KOOPA_GREEN_STATE_DIE_AND_MOVE_UP;
		vx = 0.1 * nx;
		break;
	case KOOPA_GREEN_STATE_REVIVE:
		last_state = KOOPA_GREEN_STATE_REVIVE;
		break;
	case KOOPA_GREEN_STATE_REVIVE_UP:
		last_state = KOOPA_GREEN_STATE_REVIVE_UP;
		break;
	case KOOPA_GREEN_STATE_HAS_WING_FLY_RIGHT:
		vx = KOOPAS_WALKING_SPEED + 0.01;
		vy = -0.18;
		break;
	case KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT:
		vx = -KOOPAS_WALKING_SPEED + 0.01;
		vy = -0.18;
		break;
	case KOOPA_GREEN_STATE_HOLDING:
		last_state = KOOPA_GREEN_STATE_HOLDING;
		break;
	case KOOPA_GREEN_STATE_HOLDING_UP:
		last_state = KOOPA_GREEN_STATE_HOLDING_UP;
		break;
	}

}