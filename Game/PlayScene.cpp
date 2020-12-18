#include "PlayScene.h"
#include "Textures.h"
#include "Point.h"
#include "Game.h"

#define OBJECT_TYPE_MARIO		0
#define OBJECT_TYPE_BRICK		1
#define OBJECT_TYPE_GATE		2
#define OBJECT_TYPE_KOOPA_RED	3
#define OBJECT_TYPE_CBRICK		4
#define OBJECT_TYPE_MUSHROOM	5
#define OBJECT_TYPE_LEAF		6
#define	OBJECT_TYPE_VENUS_RED	7
#define OBJECT_TYPE_VENUS_NO_FIRE 8
#define OBJECT_TYPE_VENUS_GREEN	9
#define OBJECT_TYPE_COIN		10
#define OBJECT_TYPE_GOOMBA		11
#define OBJECT_TYPE_MONEY		12
#define OBJECT_TYPE_BROKEN_BRICK	13
#define OBJECT_TYPE_KOOPA_GREEN		14
#define OBJECT_TYPE_BRICKSTAND	15
#define OBJECT_TYPE_P	16
#define CAMERA_HEIGHT_1 245
#define CAMERA_HEIGHT_2 184
#define CAMERA_SPEED 0.5f
#define GROUND_HEIGHT 520


PlayScene::PlayScene() : Scene()
{
	keyHandler = new PlayScenceKeyHandler(this);
	LoadBaseObjects();
	ChooseMap(STAGE_1);
	Game::GetInstance()->ResetTimer();
	
}

void PlayScene::LoadBaseObjects()
{
	texturesFilePath = ToLPCWSTR("Resources/Scene/base.txt");
	LoadBaseTextures();
	if (player == NULL)
	{
		player = new Player(1900, 300);
		DebugOut(L"[INFO] player CREATED! \n");
	}
	if (bullet1 == NULL)
	{
		bullet1 = new MarioBullet();
		listBullets.push_back(bullet1);
		DebugOut(L"[INFO] Bullet1 CREATED! \n");
	}
	if (bullet2 == NULL)
	{
		bullet2 = new MarioBullet();
		listBullets.push_back(bullet2);
		DebugOut(L"[INFO] Bullet2 CREATED! \n");
	}
	if (tail == NULL)
	{
		tail = new RaccoonTail();
		DebugOut(L"[INFO] tail CREATED! \n");
	}
}

void PlayScene::ChooseMap(int whatMap)
{
	idStage = whatMap;
	Game::GetInstance()->SetKeyHandler(this->GetKeyEventHandler());
	int convertSimple = idStage / STAGE_1;
	sceneFilePath = listSceneFilePath[convertSimple - 1];
	LoadSceneObjects();
}

void PlayScene::Update(DWORD dt)
{
	Game* game = Game::GetInstance();
	game->TimerTick(dt);

	if (listItems.size() > 0)
		PlayerCollideItem();
	PlayerGotGate();
	PlayerTouchItem();
	PlayerTailAttackEnemy();

#pragma region Objects Updates
	vector<LPGAMEENTITY> coObjects;
	vector<LPGAMEENTITY> fullObjects;
	vector<LPGAMEENTITY> enemies;
	vector<LPGAMEENTITY> coObjects2;
	for (int i = 0; i < listObjects.size(); i++)
	{
		coObjects.push_back(listObjects[i]);
		fullObjects.push_back(listObjects[i]);
	}
	for (int i = 0; i < listEnemies.size(); i++)
	{
		fullObjects.push_back(listEnemies[i]);
		enemies.push_back(listEnemies[i]);
	}
	player->Update(dt, &fullObjects);
	for (int i = 0; i < listBullets.size(); i++)
		listBullets[i]->Update(dt, &fullObjects);
	for (int i = 0; i < listEnemies.size(); i++)
		listEnemies[i]->Update(dt, &coObjects);
	for (int i = 0; i < listObjects.size(); i++)
	{
		if (dynamic_cast<Brick*>(listObjects[i]))
		{ }
		else
			listObjects[i]->Update(dt, &fullObjects);
	}
	for (int i = 0; i < listLeaf.size(); i++)
		listLeaf[i]->Update(dt, &coObjects2);
	for (int i = 0; i < listCoins.size(); i++)
		listCoins[i]->Update(dt, &coObjects2);
	for (int i = 0; i < listitems.size(); i++)
		listitems[i]->Update(dt, &coObjects);
	tail->Update(dt, &fullObjects, player->x, player->y);
#pragma endregion

#pragma region Camera
	if (isHiddenArea)
	{
		game->SetCamPos(2050, 465);
	}
	else
	{
		if (!player) return;

		float marioBox = player->level == MARIO_LEVEL_SMALL ? MARIO_SMALL_BBOX_HEIGHT : MARIO_BIG_BBOX_HEIGHT;

		float cx, cy;
		player->GetPosition(cx, cy);
		cx -= SCREEN_WIDTH / 2;

		if ((player->flyTrip && player->level == MARIO_LEVEL_RACCOON) || player->Gety() + marioBox / 2 > player->dGround) {
			cy = (cy + marioBox) - (float)SCREEN_HEIGHT * 0.6;
		}
		else {
			cy = player->dGround + marioBox / 2 - (float)SCREEN_HEIGHT * 0.6;
		}

		BoundaryConstraint(cx, cy);

		game->SetCamPos(cx, cy);
	}
#pragma endregion
}

void PlayScene::BoundaryConstraint(float &x, float &y) {
	if (x < 0)
	{
		x = 0;
	}
	else if (x + SCREEN_WIDTH >= tilemap->GetWidthTileMap())
	{
		x = tilemap->GetWidthTileMap() - SCREEN_WIDTH;
	}

	if (y < 0) {
		y = 0;
	}
	else if (y + SCREEN_HEIGHT > GROUND_HEIGHT) {
		y = GROUND_HEIGHT - SCREEN_HEIGHT;
	}

}

void PlayScene::PlayerTailAttackEnemy()
{
	for (UINT i = 0; i < listEnemies.size(); i++)
	{
		if (listEnemies[i]->GetType() == EntityType::GOOMBA)
		{
			auto goomba = dynamic_cast<Goomba*>(listEnemies[i]);
			if (goomba->id_goomba == GOOMBA_NORMAL)
			{
				if (tail->IsCollidingObject(listEnemies[i]))
				{
					listEnemies[i]->SetState(GOOMBA_STATE_DIE_FLY);
					goomba->make100 = true;
				}
			}
			else if (goomba->id_goomba == GOOMBA_RED)
			{
				if (tail->IsCollidingObject(listEnemies[i]))
				{
					listEnemies[i]->SetState(GOOMBA_RED_STATE_NO_WING_DIE_FLY);
					goomba->make100 = true;
				}
			}
		}
		else if (listEnemies[i]->GetType() == EntityType::KOOPA)
		{
			auto koopa = dynamic_cast<Koopa*>(listEnemies[i]);
			if (koopa->id_koopa == KOOPA_RED) {
				if (tail->IsCollidingObject(listEnemies[i]))
					listEnemies[i]->SetState(KOOPA_RED_STATE_DIE_UP);
			}
			else if (koopa->id_koopa == KOOPA_GREEN)
			{
				if (tail->IsCollidingObject(listEnemies[i]))
					listEnemies[i]->SetState(KOOPA_GREEN_STATE_DIE_UP);
				//koopa->hasWing = false;
			}
			koopa->hitByTail = true;
		}
		else if (listEnemies[i]->GetType() == EntityType::VENUS)
		{
			Venus* venus = dynamic_cast<Venus*>(listEnemies[i]);
			if (tail->IsCollidingObject(venus))
			{
				venus->isDeath = true;
			}
		}
		else if (listEnemies[i]->GetType() == EntityType::VENUSGREEN)
		{
			Venus* venus = dynamic_cast<Venus*>(listEnemies[i]);
			if (tail->IsCollidingObject(venus))
			{
				venus->isDeath = true;
			}
		}
		else if (listEnemies[i]->GetType() == EntityType::VENUSNOFIRE)
		{
			VenusNoFire* venusNoFire = dynamic_cast<VenusNoFire*>(listEnemies[i]);
			if (tail->IsCollidingObject(venusNoFire))
			{
				venusNoFire->isDeath = true;
			}
		}
		
	}
	for (UINT i = 0; i < listObjects.size(); i++)
	{
		if (listObjects[i]->GetType() == EntityType::BROKENBRICK)
		{
			BrokenBrick* brokenBrick = dynamic_cast<BrokenBrick*>(listObjects[i]);

			if (tail->IsCollidingObject(brokenBrick))
			{
				if (listObjects[i]->x == 2032 && listObjects[i]->y == 368)
				{
					for (UINT i = 0; i < listitems.size(); i++)
					{
						if (listitems[i]->GetType() == EntityType::P)
						{

							ItemP* itemP = dynamic_cast<ItemP*>(listitems[i]);
							itemP->isCollis = true;
						}

					}

					return;
				}
				brokenBrick->SetState(STATE_DESTROYED);
			}
		}
		if (listObjects[i]->GetType() == EntityType::CBRICK)
		{
			CBrick* cBrick = dynamic_cast<CBrick*>(listObjects[i]);

			if (tail->IsCollidingObject(cBrick))
			{
				cBrick->SetState(CBRICK_STATE_COLLISION);
			}
		}
	}

	for (UINT i = 0; i < listitems.size(); i++)
	{
		if (listitems[i]->GetType() == EntityType::MUSH)
		{
			if (tail->IsCollidingObject(listitems[i]) && player->level == MARIO_LEVEL_SMALL)
			{
				Mushroom* mush = dynamic_cast<Mushroom*>(listitems[i]);
				if (mush->isOnTop == false)
				{
					listitems[i]->SetState(MUSHROOM_STATE_WALKING);
				}
			}
		}
		if (listitems[i]->GetType() == EntityType::MONEY)
		{
			if (tail->IsCollidingObject(listitems[i]))
			{
				Money* money = dynamic_cast<Money*>(listitems[i]);
				if (money->isOnTop == false)
					listitems[i]->SetState(MONEY_STATE_WALKING);
			}
		}
	}
	for (UINT i = 0; i < listLeaf.size(); i++)
	{

		if (listLeaf[i]->GetType() == EntityType::LEAF && (player->level == MARIO_LEVEL_BIG || player->level == MARIO_LEVEL_RACCOON) && isCheckMushroom == false)
		{
			if (tail->IsCollidingObject(listLeaf[i]) && player->level)
			{
				Leaf* leaf = dynamic_cast<Leaf*>(listLeaf[i]);
				if (leaf->isOnTop == false)
					listLeaf[i]->SetState(LEAF_STATE_WALKING);
			}
		}
	}

}

void PlayScene::DarkenTheScreen()
{
	Game* game = Game::GetInstance();
	LPDIRECT3DTEXTURE9 darken = CTextures::GetInstance()->Get(-200);
	RECT rect;

	float l = game->GetCamPosX();
	float t = game->GetCamPosY();

	rect.left = 0;
	rect.top = 0;
	rect.right = SCREEN_WIDTH;
	rect.bottom = SCREEN_HEIGHT;
	if (isDark)
	{
		DebugOut(L"isDark");
		colorSubtrahend += 0.3;
		alpha = floor(alpha + colorSubtrahend);
	}
	if (isLight)
	{
		colorSubtrahend += 0.7;
		alpha = floor(alpha - colorSubtrahend);
	}
	if (alpha > 255)
	{

		alpha = 255;
		player->screenDim = false;
		player->inStartOfPipe = false;
		/*if (player->goHiddenArea)
		{
			cam->goToHiddenArea = true;
			player->goHiddenArea = false;
		}
		else if (player->leaveHiddenArea)
		{
			cam->backFromHiddenArea = true;
			player->leaveHiddenArea = false;
		}*/

		isLight = true;
		isDark = false;
		colorSubtrahend = 0;
		if (!isHiddenArea)
			isHiddenArea = true;
		else
			isHiddenArea = false;
	
	}
	if (alpha < 0)
	{
		alpha = 0;
		//player->screenDim = false;
		//player->inStartOfPipe = false;
		/*if (player->goHiddenArea)
		{
			cam->goToHiddenArea = true;
			player->goHiddenArea = false;
		}
		else if (player->leaveHiddenArea)
		{
			cam->backFromHiddenArea = true;
			player->leaveHiddenArea = false;
		}*/
		isLight = false;
		isDark = false;
		colorSubtrahend = 0;
		if (isHiddenArea)
		{
			player->SetPosition(2105, 495);
			player->SetState(MARIO_STATE_IDLE);
			player->isOnPipe = false;
			player->vy = 0;
		}
		else
		{
			player->SetPosition(2329, 378);
			player->SetState(MARIO_STATE_OUT_PIPE);
			player->isOnPipe = true;
		}
	}
	game->OldDraw(l, t, darken, rect.left, rect.top, rect.right, rect.bottom, alpha);
}

void PlayScene::LightTheScreen()
{
	Game* game = Game::GetInstance();
	LPDIRECT3DTEXTURE9 darken = CTextures::GetInstance()->Get(-200);
	RECT rect;

	float l = game->GetCamPosX();
	float t = game->GetCamPosY();

	rect.left = 0;
	rect.top = 0;
	rect.right = ceil(5 * SCREEN_WIDTH / 14);
	rect.bottom = ceil(5 * SCREEN_HEIGHT / 14);

	colorSubtrahend += 1.4;
	alpha = floor(alpha - colorSubtrahend);
	if (alpha < 0)
	{
		alpha = 0;
		player->screenDim = false;
		player->inStartOfPipe = false;
		/*if (player->goHiddenArea)
		{
			cam->goToHiddenArea = true;
			player->goHiddenArea = false;
		}
		else if (player->leaveHiddenArea)
		{
			cam->backFromHiddenArea = true;
			player->leaveHiddenArea = false;
		}
		colorSubtrahend = 0;
		lighteningIsDone = false;*/
		isLight = false;
		isDark = true;
	}

	game->OldDraw(l, t, darken, rect.left, rect.top, rect.right, rect.bottom, alpha);
}

bool PlayScene::PlayerPassingStage(float DistanceXWant, int directionGo)
{
	if (directionGo == 1)	//cua o ben phai
	{
		if (player->Getx() < DistanceXWant)
		{
			player->SetDirection(directionGo);
			player->SetState(MARIO_STATE_IDLE);
			return false;
		}
	}
	else
		if (directionGo == -1)	//cua o ben trai
		{
			if (player->Getx() > DistanceXWant)
			{
				player->SetDirection(directionGo);
				player->SetState(MARIO_STATE_IDLE);
				return false;
			}
		}
	return true;
}

void PlayScene::PlayerTouchItem()
{
	for (UINT i = 0; i < listitems.size(); i++)
	{ 
		if (listitems[i]->GetType() == EntityType::MUSH)
		{
			if (player->IsCollidingObject(listitems[i]) && player->level == MARIO_LEVEL_SMALL)
			{
				Mushroom* mush = dynamic_cast<Mushroom*>(listitems[i]);
				if (mush->isOnTop == false)
				{
					listitems[i]->SetState(MUSHROOM_STATE_WALKING);
				}
				else
				{
					player->SetLevel(MARIO_LEVEL_BIG);
					player->y -= 13;
					mush->make100 = true;
					Game::GetInstance()->Score += 100;
					isCheckMushroom = true;
				}
				
			}
		}
		if (listitems[i]->GetType() == EntityType::MONEY)
		{
			if (player->IsCollidingObject(listitems[i]))
			{
				Money* money = dynamic_cast<Money*>(listitems[i]);
				if (money->isOnTop == false)
					listitems[i]->SetState(MONEY_STATE_WALKING);
			}
		}
		if (listitems[i]->GetType() == EntityType::COIN)
		{
			if (player->IsCollidingObject(listitems[i]))
			{
				Coin* coin = dynamic_cast<Coin*>(listitems[i]);
				/*if (money->isOnTop == false)
					listitems[i]->SetState(MONEY_STATE_WALKING);*/
				coin->SetDone(true);
			}
		}
		if (listitems[i]->GetType() == EntityType::P)
		{
			if (player->IsCollidingObject(listitems[i]))
			{
				ItemP* itemP = dynamic_cast<ItemP*>(listitems[i]);
				if (!itemP->isCollis)
				{
					itemP->isCollis = true;
					Entity *obj = new CBrick(2032, 368, 16, 16);
					CAnimationSets* animation_sets = CAnimationSets::GetInstance();
					LPANIMATION_SET ani_set = animation_sets->Get(4);
					obj->SetAnimationSet(ani_set);
					CBrick* cBrick = dynamic_cast<CBrick*>(obj);
					for (UINT i = 0; i < listObjects.size(); i++)
					{
						//DebugOut(L" show list object %d \n", listObjects[i]->tag);
						if (listObjects[i]->tag == EntityType::BROKENBRICK)
						{
							BrokenBrick* brick = dynamic_cast<BrokenBrick*>(listObjects[i]);
							if (brick->x == 2032 && brick->y == 368) // cuc gach do nut P
							{
								brick->SetState(STATE_HIDE);
							}
						}
					}
					cBrick->SetState(CBRICK_STATE_COLLISION);
					listObjects.push_back(obj);

					//DebugOut(L"dap nut lan 1 \n");
				}
				else
				{
					//DebugOut(L"isOnTop = %d \n", itemP->isOnTop);
					if (itemP->isOnTop)
					{
						//DebugOut(L"dap nut lan 1");
						itemP->isDone = true;
						if (!isCheckLoadCoin)
						{
							for (UINT i = 0; i < listObjects.size(); i++)
							{
								//DebugOut(L" show list object %d \n", listObjects[i]->tag);
								if (listObjects[i]->tag == EntityType::BROKENBRICK)
								{
									BrokenBrick* brick = dynamic_cast<BrokenBrick*>(listObjects[i]);
									if (brick->x == 2032 && brick->y == 368) // cuc gach do nut P
									{
										continue;
									}
									else
									{
										if (!brick->isDestroyed)
										{
											brick->SetState(STATE_HIDE);
											//DebugOut(L"Hide 1");
											float x = brick->x;
											float y = brick->y;
											CAnimationSets* animation_sets = CAnimationSets::GetInstance();

											Entity* obj = NULL;
											obj = new Coin(x, y);
											obj->SetPosition(x, y);
											LPANIMATION_SET ani_set = animation_sets->Get(16);

											obj->SetAnimationSet(ani_set);
											obj->detailTag = COINBRICK;
											listitems.push_back(obj);
											//DebugOut(L"[test] add coin !\n");
										}
									}
								}
							}
							isCheckLoadCoin = true;
							countP = 0;
						}
					}
				}
			}
			if (isCheckLoadCoin == true)
			{
				if (countP <400)
					countP += 1;
				else
				{
					for (UINT i = 0; i < listitems.size(); i++)
					{
						//DebugOut(L" show list object %d \n", listObjects[i]->tag);
						if (listitems[i]->tag == EntityType::COIN && listitems[i]->detailTag == COINBRICK)
						{
							Coin* coin = dynamic_cast<Coin*>(listitems[i]);
							if (!coin->isdone)
							{
								coin->SetDone(true);
								//DebugOut(L"Hide 1");
								float x = coin->x;
								float y = coin->y;
								CAnimationSets* animation_sets = CAnimationSets::GetInstance();

								Entity* obj = NULL;
								obj = new BrokenBrick(1);
								obj->SetPosition(x, y);
								LPANIMATION_SET ani_set = animation_sets->Get(20);

								obj->SetAnimationSet(ani_set);
								listObjects.push_back(obj);
								//DebugOut(L"[test] add broken brick !\n");
							}
						}
					}
					isCheckLoadCoin == false;
				}
				//DebugOut(L"%d \n", countP);
			}
		}
	}
	for (UINT i = 0; i < listLeaf.size(); i++)
	{

		if (listLeaf[i]->GetType() == EntityType::LEAF && player->level == MARIO_LEVEL_BIG && isCheckMushroom == false)
		{
			if (player->IsCollidingObject(listLeaf[i]) && player->level)
			{
				Leaf* leaf = dynamic_cast<Leaf*>(listLeaf[i]);
				if (leaf->isOnTop == false)
					listLeaf[i]->SetState(LEAF_STATE_WALKING);
				else
				{
					player->SetLevel(MARIO_LEVEL_RACCOON);
					leaf->isDeath = true;
					leaf->make100 = true;
					Game::GetInstance()->Score += 100;
				}
			}
		}
	}
}

void PlayScene::PlayerGotGate()
{
	Game* game = Game::GetInstance();
	for (UINT i = 0; i < listObjects.size(); i++)
	{
		if (listObjects[i]->GetType() == EntityType::GATE)
		{
			if (player->IsCollidingObject(listObjects[i]))
			{
        		Gate* gate = dynamic_cast<Gate*>(listObjects[i]);
				int tempMap = gate->GetIdScene() * STAGE_1;
				float tempx = gate->newPlayerx;
				float tempy = gate->newPlayery;
				int tempState = gate->newPlayerState;
				bool tempNeed = gate->isNeedResetCam;
				/*if (idStage == STAGE_1)
				{
					if (!PlayerPassingStage(listObjects[i]->Getx() + 20.0f, 1))
						return;
				}
				else if (idStage == STAGE_2_2 && tempMap == STAGE_3_1)
				{
					if (!PlayerPassingStage(listObjects[i]->Getx(), -1))
						return;
				}
				else if (idStage == STAGE_3_2 && tempMap == STAGE_4)
				{
					if (!PlayerPassingStage(listObjects[i]->Getx() + 10.0f, 1))
						return;
				}*/
				Unload();
				if(tempNeed)
					game->SetCamPos(0, 0);
				ChooseMap(tempMap);
				player->SetPosition(tempx, tempy);
				player->Setvx(0);
				player->Setvy(0);
				player->SetState(tempState);
			}
		}
	}
}

void PlayScene::PlayerCollideItem()
{
	for (UINT i = 0; i < listItems.size(); i++)
	{
		if (!listItems[i]->GetIsDone())
		{
			if (player->IsCollidingObject(listItems[i]))
			{
				switch (listItems[i]->GetType())
				{
				/*case EntityType::POWERUP:
				{
					if (player->GetHealth() + POWER_HP_RESTORE <= MAX_HEALTH)
						player->AddHealth(POWER_HP_RESTORE);
					else
						player->SetHealth(MAX_HEALTH);
					listItems[i]->SetIsDone(true);
					break;
				}
				case EntityType::GUNUP:
				{
					if (player->GetgunDam() + GUN_HP_RESTORE <= MAX_HEALTH)
						player->AddgunDam(GUN_HP_RESTORE);
					else
						player->SetgunDam(MAX_HEALTH);
					listItems[i]->SetIsDone(true);
					break;
				}*/
				default:
					break;
				}
			}
		}
	}
}

void PlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	Player* player = ((PlayScene*)scence)->player;
	Bullet* bullet1 = ((PlayScene*)scence)->bullet1;
	Bullet* bullet2 = ((PlayScene*)scence)->bullet2;
	PlayScene* playScene = dynamic_cast<PlayScene*>(scence);
	vector<LPGAMEENTITY> listitems = ((PlayScene*)scence)->listitems;
	vector<LPGAMEENTITY> listObjects = ((PlayScene*)scence)->listObjects;
	vector<LPGAMEENTITY> listEnemies = ((PlayScene*)scence)->listEnemies;
	vector<LPGAMEENTITY> listLeaf = ((PlayScene*)scence)->listLeaf;
	vector<LPBULLET> listBullets = ((PlayScene*)scence)->listBullets;
	float x, y;
	int direction;
	player->GetInfoForBullet(direction, x, y);
	switch (KeyCode)
	{
	case DIK_S:
		player->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_DOWN:
		player->keyDown == true;
		if (player->y > 80 && player->y < 97 && player->x > 2263 && player->x <2270)
		{
			player->isOnPipe = true;
			player->SetState(MARIO_STATE_INTO_PIPE);
			playScene->isDark = true;
		}
		break;
	case DIK_UP:
		player->keyUp == true;
		if (player->y > 497 && player->y < 500 && player->x > 2325 && player->x < 2335 && playScene->isHiddenArea == true)
		{
			player->isOnPipe = true;
			player->SetState(MARIO_STATE_OUT_PIPE);
			playScene->isDark = true;
		}
		break;
	case DIK_Z:
		player->isAttack = true;
		break;
	case DIK_V:
		if (player->level != MARIO_LEVEL_SMALL)
			player->y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		player->level = MARIO_LEVEL_SMALL;
		break;
	case DIK_B:
		if (player->level == MARIO_LEVEL_SMALL)
			player->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		player->level = MARIO_LEVEL_BIG;
		break;
	case DIK_N:
		if (player->level == MARIO_LEVEL_SMALL)
			player->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		player->level = MARIO_LEVEL_RACCOON;
		break;
	case DIK_M:
		if (player->level == MARIO_LEVEL_SMALL)
			player->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
		player->level = MARIO_LEVEL_FIRE;
		break;
	case DIK_A:
		player->isAttack = true;
		if (player->level == MARIO_LEVEL_FIRE)
		{
			player->isBullet = true;
			player->shootTime = GetTickCount64();
			for (int i = 0; i < listBullets.size(); i++)
			{
				if (listBullets[i]->isDone == true)
				{
					listBullets[i]->Fire(direction, x, y);
					break;
				}
			}
		}
		else if (player->level == MARIO_LEVEL_RACCOON)
		{
			//playScene->tail->Attack(x - 8, y + 19);
			playScene->tail->Attack();
		}
		break;
	case DIK_F6:
		for (int i = 0; i < listObjects.size(); i++)
		{
			if (listObjects[i]->GetBBARGB() == 0)
				listObjects[i]->SetBBARGB(200);
			else
				listObjects[i]->SetBBARGB(0);
		}
		for (int i = 0; i < listEnemies.size(); i++)
		{
			if (listEnemies[i]->GetBBARGB() == 0)
				listEnemies[i]->SetBBARGB(200);
			else
				listEnemies[i]->SetBBARGB(0);
		}
		for (int i = 0; i < listLeaf.size(); i++)
		{
			if (listLeaf[i]->GetBBARGB() == 0)
				listLeaf[i]->SetBBARGB(200);
			else
				listLeaf[i]->SetBBARGB(0);
		}
		for (int i = 0; i < listitems.size(); i++)
		{
			if (listitems[i]->GetBBARGB() == 0)
				listitems[i]->SetBBARGB(200);
			else
				listitems[i]->SetBBARGB(0);
		}


		if (player->GetBBARGB() == 0)
			player->SetBBARGB(200);
		else
			player->SetBBARGB(0);

		if (bullet1->GetBBARGB() == 0)
			bullet1->SetBBARGB(200);
		else
			bullet1->SetBBARGB(0);

		if (bullet2->GetBBARGB() == 0)
		if (bullet2->GetBBARGB() == 0)
			bullet2->SetBBARGB(200);
		else
			bullet2->SetBBARGB(0);
		break;
	}
}

void PlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	//DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	Player* player = ((PlayScene*)scence)->player;
	PlayScene* playScene = dynamic_cast<PlayScene*>(scence);
	vector<LPGAMEENTITY> listObj = ((PlayScene*)scence)->listObjects;
	float posX, posY;
	switch (KeyCode)
	{
	case DIK_S:
		player->SetPressS(false);
		break;
	case DIK_RIGHT:
		player->startWalkingDone();
		player->walkingDirection = 1;
		break;
	case DIK_LEFT:
		player->startWalkingDone();
		player->walkingDirection = -1;
		break;
	case DIK_DOWN:
		player->isCrouch = false;
		player->keyDown = false;
		break;
	case DIK_UP:
		player->keyUp = false;
		break;
	case DIK_A:
		if (player->holdthing)
		{
			player->holdthing->nx = -player->nx;
			Koopa* koopa = dynamic_cast<Koopa*>(player->holdthing);
			if (koopa != nullptr) {
				player->isKick = true;
				if (koopa->id_koopa == KOOPA_RED)
				{
					player->holdthing->SetState(KOOPA_RED_STATE_DIE_AND_MOVE);
				}
				else if (koopa->id_koopa == KOOPA_GREEN)
				{
					player->holdthing->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE);
				}
			}
			
		}
		player->isRun = false;
		break;
	}
}

void PlayScenceKeyHandler::KeyState(BYTE* states)
{
	Player* player = ((PlayScene*)scence)->player;
	Bullet* bullet1 = ((PlayScene*)scence)->bullet1;
	Bullet* bullet2 = ((PlayScene*)scence)->bullet2;

	//Bullet* bullet3 = ((PlayScene*)scence)->bullet3;
	//Bullet* supBullet = ((PlayScene*)scence)->supBullet;

	PlayScene* playScene = dynamic_cast<PlayScene*>(scence);
	vector<LPGAMEENTITY> listObjects = ((PlayScene*)scence)->listObjects;
	vector<LPGAMEITEM> listItems = ((PlayScene*)scence)->listItems;
	vector<LPBULLET> listBullets = ((PlayScene*)scence)->listBullets;
	if (player->isOnPipe)
		return;
	if (player->GetState() == MARIO_STATE_DIE) return;

	if (Game::GetInstance()->IsKeyDown(DIK_RIGHT) && !player->isWalkingComplete)
	{
		player->SetState(MARIO_STATE_WALKING_RIGHT);
		if (Game::GetInstance()->IsKeyDown(DIK_A))
		{
			player->isRun = true;
		}
	}
	else if (Game::GetInstance()->IsKeyDown(DIK_LEFT) && !player->isWalkingComplete)
	{
		player->SetState(MARIO_STATE_WALKING_LEFT);
		if (Game::GetInstance()->IsKeyDown(DIK_A))
		{
			player->isRun = true;
		}
	}
	else if (Game::GetInstance()->IsKeyDown(DIK_DOWN))
	{
		player->SetState(MARIO_STATE_CROUCH);
	}
	else if (Game::GetInstance()->IsKeyDown(DIK_Z))
	{
		player->SetState(MARIO_STATE_SPIN);
	}
	else
	{
		if (player->isJumping)
			return;
		player->SetState(MARIO_STATE_IDLE);
		player->isCrouch = false;
	}

	if (Game::GetInstance()->IsKeyDown(DIK_S))
	{
		player->SetPressS(true);
	}

}

void PlayScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void PlayScene::_ParseSection_CLEARTEXTURES(string line)
{
	vector<string> tokens = split(line);
	int idClear = atoi(tokens[0].c_str());
	CTextures::GetInstance()->ClearAt(idClear);
	DebugOut(L"[INFO] Cleared Texture %d!\n", idClear);
}

void PlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str()) + l;
	int b = atoi(tokens[4].c_str()) + t;
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void PlayScene::_ParseSection_CLEARSPRITES(string line)
{
	vector<string> tokens = split(line);
	int idClear = atoi(tokens[0].c_str());
	CSprites::GetInstance()->ClearAt(idClear);
	DebugOut(L"[INFO] Cleared Sprite %d!\n", idClear);
}

void PlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void PlayScene::_ParseSection_CLEARANIMATIONS(string line)
{
	vector<string> tokens = split(line);
	int idClear = atoi(tokens[0].c_str());
	//CAnimations::GetInstance()->ClearAt(idClear);
	DebugOut(L"[INFO] Cleared Animation %d!\n", idClear);
}

void PlayScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < tokens.size(); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
		DebugOut(L"add animation set %d anmation %d \n", ani_set_id, ani_id);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

void PlayScene::_ParseSection_CLEARANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);
	int idClear = atoi(tokens[0].c_str());
	//CAnimationSets::GetInstance()->ClearAt(idClear);
	DebugOut(L"[INFO] Cleared Animation Set %d!\n", idClear);
}

void PlayScene::_ParseSection_TILEMAP(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	wstring filePath_texture = ToWSTR(tokens[1]);
	wstring filePath_data = ToWSTR(tokens[2]);
	int num_row_on_texture = atoi(tokens[3].c_str());
	int num_col_on_textture = atoi(tokens[4].c_str());
	int num_row_on_tilemap = atoi(tokens[5].c_str());
	int num_col_on_tilemap = atoi(tokens[6].c_str());
	int tileset_width = atoi(tokens[7].c_str());
	int tileset_height = atoi(tokens[8].c_str());
	//int widthGrid = atoi(tokens[9].c_str());
	//int heightGrid = atoi(tokens[10].c_str());
	//boardscore = new BoardScore();
	//grid = new Grid();
	//grid->Resize(widthGrid, heightGrid);
	//grid->PushGrid(allObject);
	tilemap = new TileMap(ID, filePath_texture.c_str(), filePath_data.c_str(), num_row_on_texture, num_col_on_textture, num_row_on_tilemap, num_col_on_tilemap, tileset_width, tileset_height);
}

void PlayScene::_ParseSection_SCENEFILEPATH(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return;

	listSceneFilePath.push_back(ToLPCWSTR(tokens[0]));
	mapWidth = atoi(tokens[1].c_str());
	mapHeight = atoi(tokens[2].c_str());
	//Hien tai chi lay mapWidth/Height cua list cuoi cung` :P co the dem vo tung file scene rieng de phan biet (hoac khong can, camera co chay toi duoc dau)
}

void PlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = atof(tokens[1].c_str());
	float y = atof(tokens[2].c_str());
	int ani_set_id = atoi(tokens[3].c_str());
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	Entity* obj = NULL;
	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new Player(x, y);
		player = (Player*)obj;

		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_BRICK:
	{
		obj = new Brick(atof(tokens[4].c_str()),atof(tokens[5].c_str()));
		obj->SetPosition(x, y);
		listObjects.push_back(obj);
		DebugOut(L"[test] add brick !\n");
		break;
	}
	case OBJECT_TYPE_CBRICK:
	{
		obj = new CBrick(x,y,atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		//obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		
		obj->SetAnimationSet(ani_set);
		listObjects.push_back(obj);
		DebugOut(L"[test] add cbrick !\n");
		break;
	}
	case OBJECT_TYPE_BRICKSTAND:
	{
		obj = new BrickStand(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		obj->SetPosition(x, y);
		listObjects.push_back(obj);
		DebugOut(L"[test] add brick !\n");
		break;
	}
	case OBJECT_TYPE_BROKEN_BRICK:
	{
		int id_state = atoi(tokens[4].c_str());
		obj = new BrokenBrick(id_state);
		obj->SetPosition(x, y);
		/*LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);*/
		listObjects.push_back(obj);
		DebugOut(L"[test] add brick broken !\n");
		//obj->id_broken_state= atoi(tokens[6].c_str());
		break;
	}
	case OBJECT_TYPE_P:
	{
		obj = new ItemP(x,y);
		obj->SetPosition(x, y);
		listitems.push_back(obj);
		DebugOut(L"[test] add point !\n");
		break;
	}
	case OBJECT_TYPE_KOOPA_RED:
	{
		obj = new Koopa(player, 1);
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add koopa !\n");
		break;
	}
	case OBJECT_TYPE_KOOPA_GREEN:
	{
		obj = new Koopa(player, 2);
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add koopa !\n");
		break;
	}
	case OBJECT_TYPE_VENUS_RED:
	{
		obj = new Venus(player, 1);
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add venus !\n");
		break;
	}
	case OBJECT_TYPE_VENUS_GREEN:
	{
		obj = new Venus(player, 2);
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add venus !\n");
		break;
	}
	case OBJECT_TYPE_VENUS_NO_FIRE:
	{
		obj = new VenusNoFire(player);
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add venus !\n");
		break;
	}
	case OBJECT_TYPE_GOOMBA:
	{
		obj = new Goomba(player);
		obj->id_goomba = atoi(tokens[6].c_str());
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listEnemies.push_back(obj);
		DebugOut(L"[test] add goomba !\n");
		break;
	}
	case OBJECT_TYPE_MUSHROOM:
	{
		obj = new Mushroom(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listitems.push_back(obj);
		DebugOut(L"[test] add mushroom !\n");
		break;
	}
	case OBJECT_TYPE_LEAF:
	{
		obj = new Leaf(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listLeaf.push_back(obj);
		//listObjects.push_back(obj);
		DebugOut(L"[test] add leaf !\n");
		break;
	}
	case OBJECT_TYPE_MONEY:
	{
		obj = new Money(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listitems.push_back(obj);
		DebugOut(L"[test] add money !\n");
		break;
	}
	case OBJECT_TYPE_COIN:
	{
		obj = new Coin(atof(tokens[4].c_str()), atof(tokens[5].c_str()));
		obj->SetPosition(x, y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

		obj->SetAnimationSet(ani_set);
		listitems.push_back(obj);
		DebugOut(L"[test] add coin !\n");
		break;
	}
	case OBJECT_TYPE_GATE:
	{
		int switchId = atoi(tokens[3].c_str());
		float playerPosX = atoi(tokens[4].c_str());
		float playerPosY = atoi(tokens[5].c_str());
		int playerState = atoi(tokens[6].c_str());
		int isResetCamera = atoi(tokens[7].c_str());
		obj = new Gate(x, y, switchId, playerPosX, playerPosY, playerState, isResetCamera);
		listObjects.push_back(obj);
		DebugOut(L"[test] add gate !\n");
		break;
	}

	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

}

void PlayScene::LoadBaseTextures()
{
	DebugOut(L"[INFO] Start loading TEXTURES resources from : %s \n", texturesFilePath);

	ifstream f;
	f.open(texturesFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") {
			section = SCENE_SECTION_TEXTURES; continue;
		}
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]")
		{
			section = SCENE_SECTION_OBJECTS;
			continue;
		}
		if (line == "[SCENE]") {
			section = SCENE_SECTION_SCENEFILEPATH; continue;
		}
		/*if (line == "[MAPTEXTURES]") {
			section = SCENE_SECTION_MAPTEXTURES; continue;
		}*/

		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_SCENEFILEPATH: _ParseSection_SCENEFILEPATH(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading TEXTURES resources %s\n", texturesFilePath);
}

void PlayScene::LoadSceneObjects()
{
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[CLEARTEXTURES]") {
			section = SCENE_SECTION_CLEARTEXTURES; continue;
		}
		if (line == "[CLEARSPRITES]") {
			section = SCENE_SECTION_CLEARSPRITES; continue;
		}
		if (line == "[CLEARANIMATIONS]") {
			section = SCENE_SECTION_CLEARANIMATIONS; continue;
		}
		if (line == "[CLEARANIMATIONSETS]") {
			section = SCENE_SECTION_CLEARANIMATION_SETS; continue;
		}
		if (line == "[TEXTURES]") {
			section = SCENE_SECTION_TEXTURES; continue;
		}
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line == "[TILEMAP]") {
			section = SCENE_SECTION_TILEMAP; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_CLEARTEXTURES: _ParseSection_CLEARTEXTURES(line); break;
		case SCENE_SECTION_CLEARSPRITES: _ParseSection_CLEARSPRITES(line); break;
		case SCENE_SECTION_CLEARANIMATIONS: _ParseSection_CLEARANIMATIONS(line); break;
		case SCENE_SECTION_CLEARANIMATION_SETS: _ParseSection_CLEARANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_TILEMAP:	_ParseSection_TILEMAP(line); break;
		}
	}

	f.close();
	CTextures::GetInstance()->Add(ID_TEX_BBOX, L"Resources\\bbox.png", D3DCOLOR_XRGB(255, 255, 0));
	CTextures::GetInstance()->Add(-200, L"Resources\\squarebox.png", D3DCOLOR_XRGB(255, 255, 0));
	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
}

void PlayScene::Unload()
{
	for (int i = 0; i < listObjects.size(); i++)
		delete listObjects[i];
	listObjects.clear();
	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

//Item* PlayScene::RandomItem(float x, float y)
//{
//
//	int bagrandom = rand() % 100;
//	int random = rand() % 100;
//	if (random <= 30)
//		return new PowerUp(x, y);
//	else if (30 < random && random <= 60)
//		return new PowerUp(x, y);
//	else if (60< random && random <= 100)
//		return new PowerUp(x, y);
//}
//
//Item* PlayScene::DropItem(EntityType createrType, float x, float y, int idCreater)
//{
//	if (createrType == ENEMY)
//		return new PowerUp(x, y);
//	return new PowerUp(x, y);
//}

void PlayScene::Render()
{

	tilemap->Draw();
	for (int i = 0; i < listObjects.size(); i++)
		listObjects[i]->Render();
	for (int i = 0; i < listitems.size(); i++)
		listitems[i]->Render();
	for (int i = 0; i < listLeaf.size(); i++)
		listLeaf[i]->Render();
	for (int i = 0; i < listEnemies.size(); i++)
		listEnemies[i]->Render();
	for (int i = 0; i < listCoins.size(); i++)
		listCoins[i]->Render();
	player->Render();
	//supBullet->Render();
	for (int i = 0; i < listBullets.size(); i++)
		listBullets[i]->Render();
	tail->Render();
	gameHUD->Draw();

	DarkenTheScreen();
}

