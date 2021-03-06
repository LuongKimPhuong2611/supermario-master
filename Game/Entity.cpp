#include <d3dx9.h>
#include <algorithm>


#include "debug.h"
#include "Textures.h"
#include "Game.h"
#include "Entity.h"
#include "Sprites.h"

Entity::Entity()
{
	x = y = 0;
	vx = vy = 0;
	direction = 1;
	bbARGB = 200;
}

void Entity::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	this->dt = dt;
	dx = vx * dt;
	dy = vy * dt;
}

/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT Entity::SweptAABBEx(LPGAMEENTITY coO)
{
	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	coO->GetBoundingBox(sl, st, sr, sb);

	// deal with moving object: m speed = original m speed - collide object speed
	float svx, svy;
	coO->GetSpeed(svx, svy);

	float sdx = svx * dt;
	float sdy = svy * dt;

	// (rdx, rdy) is RELATIVE movement distance/velocity 
	float rdx = this->dx - sdx;
	float rdy = this->dy - sdy;

	GetBoundingBox(ml, mt, mr, mb);

	Game::SweptAABB(
		ml, mt, mr, mb,
		rdx, rdy,
		sl, st, sr, sb,
		t, nx, ny
	);

	CCollisionEvent* e = new CCollisionEvent(t, nx, ny, rdx, rdy, coO);
	return e;
}

/*
	Calculate potential collisions with the list of colliable objects
	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void Entity::CalcPotentialCollisions(
	vector<LPGAMEENTITY>* coObjects,
	vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		/*if (!coObjects->at(i)->IsActive()) {
			continue;
		}

		if (IsOverlapped(coObjects->at(i)) && !coObjects->at(i)->IsTrigger()) {
			OnOverlapped(coObjects->at(i));
		}*/

		/*if (coObjects->at(i)->GetCollider().IsRectF0() || GetCollider().IsRectF0()) {
			continue;
		}*/

		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f)
			coEvents.push_back(e);
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void Entity::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0.0f;
	ny = 0.0f;

	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (c->t < min_tx && c->nx != 0) {
			min_tx = c->t; nx = c->nx; min_ix = i; rdx = c->dx;
		}

		if (c->t < min_ty && c->ny != 0) {
			min_ty = c->t; ny = c->ny; min_iy = i; rdy = c->dy;
		}
	}

	if (min_ix >= 0) coEventsResult.push_back(coEvents[min_ix]);
	if (min_iy >= 0) coEventsResult.push_back(coEvents[min_iy]);
}

bool Entity::IsCollidingObject(Entity* Obj)
{
	float ml, mt, mr, mb;		// moving object bbox
	this->GetBoundingBox(ml, mt, mr, mb);

	float sl, st, sr, sb;		// static object bbox
	Obj->GetBoundingBox(sl, st, sr, sb);

	//Check AABB first
	/*LPSPRITE sprt = animationSet->at(0)->GetAnimationCurrentFrame(0)->GetSprite();
	LPSPRITE coSprt = Obj->animationSet->at(0)->GetAnimationCurrentFrame(0)->GetSprite();*/
	if (Game::GetInstance()->IsCollidingAABB(
		ml, mt, mr, mb,
		sl, st, sr, sb))
		return true;

	//Swept AABB later
	LPCOLLISIONEVENT e = SweptAABBEx(Obj);
	bool isColliding = e->t > 0 && e->t <= 1;
	return isColliding;
}



RECT Entity::GetBBox()
{
	float left, top, right, bottom;
	GetBoundingBox(left, top, right, bottom);
	RECT Bbox;
	Bbox.left = (long)left;
	Bbox.top = (long)top;
	Bbox.right = (long)right;
	Bbox.bottom = (long)bottom;
	return  Bbox;
}

void Entity::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	Game::GetInstance()->OldDraw(l, t, bbox, rect.left, rect.top, rect.right, rect.bottom, bbARGB);
}




Entity::~Entity()
{
}