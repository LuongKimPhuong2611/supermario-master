#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include "TileMap.h"
#include "HUD.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Game.h"
#include "Camera.h"
#include "Item.h"
#include "Player.h"
//#include "HUD.h"
#include "Brick.h"
#include "CBrick.h"
#include "BrickStand.h"
#include "BrokenBrick.h"
#include "Koopa.h"
#include "Goomba.h"
#include "Mushroom.h"
#include "Venus.h"
#include "VenusNoFire.h"
#include "Leaf.h"
#include "Coin.h"
#include "Money.h"
#include "Gate.h"
#include "RaccoonTail.h"
#include "P.h"
//#include "PowerUp.h"
//#include "GunUp.h"

//#include "Centipede.h"
//#include "Golem.h"
//#include "Gunner.h"
//#include "Domes.h"

#include "MarioBullet.h"
//#include "ElectricBullet.h"


#include <iostream>
#include <fstream>

#include"Entity.h"

using namespace std;

class PlayScene : public Scene
{
public:

protected:
	TileMap* tilemap;
	Player* player;
	HUD* gameHUD;
	Bullet* bullet1;
	Bullet* bullet2;
	RaccoonTail* tail;
	DWORD countP = 0;
	bool isCheckLoadCoin = false;
	bool isCheckMushroom = false;
	//Bullet* bullet3;
	//Bullet* supBullet;
	//PowerUp* powerUp;
	//GunUp* gunUp;
	vector<LPGAMEENTITY> listObjects;
	vector<LPGAMEENTITY> listEnemies;
	vector<LPBULLET> listBullets;
	vector<LPCWSTR> listSceneFilePath;
	vector<LPGAMEITEM> listItems;
	vector<LPGAMEENTITY> listLeaf;
	vector<LPGAMEENTITY> listitems;
	vector<LPGAMEENTITY> listCoins;
	vector<LPGAMEENTITY> listBrick;
	int idStage;
	int mapWidth, mapHeight;
	int camMaxWidth;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_CLEARTEXTURES(string line);
	void _ParseSection_CLEARSPRITES(string line);
	void _ParseSection_CLEARANIMATIONS(string line);
	void _ParseSection_CLEARANIMATION_SETS(string line);

	//Get stage objects' link and tilemap resources
	void _ParseSection_TILEMAP(string line);
	void _ParseSection_SCENEFILEPATH(string line);


public:
	PlayScene();

	void LoadBaseObjects();
	void LoadBaseTextures();
	void ChooseMap(int whatStage);
	bool PlayerPassingStage(float DistanceXWant, int directionGo);
	void PlayerGotGate();
	void PlayerTouchItem();
	void PlayerCollideItem();
	void BoundaryConstraint(float &x, float &y);
	void PlayerTailAttackEnemy();
	virtual void LoadSceneObjects();
	virtual void Update(DWORD dt);;
	virtual void Render();
	virtual void Unload();
	Player* GetPlayer() { return player; }
	void DarkenTheScreen();
	void LightTheScreen();
	double colorSubtrahend;
	int alpha = 0;
	bool isDark = false;
	bool isLight = false;
	bool isHiddenArea = false;
	//Item* RandomItem(float x, float y);
	//Item* DropItem(EntityType createrType, float x, float y, int idCreater = 0);

	friend class PlayScenceKeyHandler;
};


class PlayScenceKeyHandler : public ScenceKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	PlayScenceKeyHandler(Scene* s) :ScenceKeyHandler(s) {};
};


