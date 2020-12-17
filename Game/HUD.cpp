#include "HUD.h"
#include "Game.h"
#include "Sprites.h"
#include "Textures.h"
#include "Player.h"
#include "SceneManager.h"
#include "PlayScene.h"
#include "Entity.h"

#include <string>
#include "define.h"

void HUD::Load()
{

}

void HUD::Draw()
{
	DrawBackground();
	DrawCard();
	DrawScore();
	DrawSpeedBar();
	DrawTags();
	DrawTimer();
}

void HUD::DrawBackground()
{
	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(-200);

	float cam_x, cam_y;
	Game::GetInstance()->GetCamPos(cam_x, cam_y);

	RECT size;
	size.left = 0;
	size.top = 0;
	size.right = size.left + SCREEN_WIDTH;
	size.bottom = size.top + SCREEN_HEIGHT;

	Game::GetInstance()->Draw(0, cam_x, cam_y + 184 + 1, tex, size.left, size.top, size.right, size.bottom, 255);
}

void HUD::DrawCard()
{
	float cam_x, cam_y;
	Game::GetInstance()->GetCamPos(cam_x, cam_y);

	auto sprites = CSprites::GetInstance();

	CSprite* hud = sprites->Get(20);
	hud->Draw(1, cam_x + 10, cam_y + 184 + 4);

	for (size_t i = 0; i < 3; i++)
	{
		LPSPRITE card = sprites->Get(21);
		card->Draw(1, cam_x + 10 + 152 + 12 + i * 24, cam_y + 184 + 4);
	}
}

void HUD::DrawScore()
{
	float cam_x, cam_y;
	Game::GetInstance()->GetCamPos(cam_x, cam_y);

	auto sprites = CSprites::GetInstance();
	LPSPRITE num = sprites->Get(0);

	int score = Game::GetInstance()->Score;
	string scoreString = NumberToString(score, 7);
	for (size_t i = 0; i < scoreString.length(); i++)
	{

		if (scoreString[i] == ' ')
			continue;

		for (size_t j = 0; j < 10; j++)
		{
			if (scoreString[i] == 48 + j)
			{
				num = sprites->Get(j);
			}
		}
		num->Draw(1, cam_x + 10 + 51 + i * 8, cam_y + 184 + 4 + 15);
	}
}

void HUD::DrawTags()
{
	float cam_x, cam_y;
	Game::GetInstance()->GetCamPos(cam_x, cam_y);

	auto sprites = CSprites::GetInstance();

	auto worldTag = sprites->Get(1);
	worldTag->Draw(1, cam_x + 10 + 36, cam_y + 184 + 4 + 7);

	auto characterTag = sprites->Get(25);
	characterTag->Draw(1, cam_x + 10 + 4, cam_y + 184 + 4 + 15);
}

void HUD::DrawTimer()
{
	float cam_x, cam_y;
	Game* game = Game::GetInstance();
	game->GetCamPos(cam_x, cam_y);


	auto sprites = CSprites::GetInstance();
	LPSPRITE num = sprites->Get(0);

	int time = (int)game->Timer;
	string scoreString = NumberToString(time, 3);
	for (size_t i = 0; i < scoreString.length(); i++)
	{

		if (scoreString[i] == ' ')
			continue;

		for (size_t j = 0; j < 10; j++)
		{
			if (scoreString[i] == 48 + j)
			{
				num = sprites->Get(j);
			}
		}
		num->Draw(1, cam_x + 10 + 124 + i * 8, cam_y + 184 + 4 + 15);
	}
}

void HUD::DrawSpeedBar()
{
	float cam_x, cam_y;
	Game::GetInstance()->GetCamPos(cam_x, cam_y);

	auto sprites = CSprites::GetInstance();

	for (int i = 0; i < 6; i++)
	{
		auto barSegment = sprites->Get(27);
		//barSegment->Draw(1, cam_x + 10 + 51 + i * 8, cam_y + 184 + 4 + 6);
	}

	PlayScene* currentScene = dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetScene());
	if (currentScene) {
		Player* player = currentScene->GetPlayer();
		if (player->GetIsRun() == false)	return;
		int speedLevel = player->GetSpeedLevel();

		auto barSegment = sprites->Get(27);
		for (int i = 0; i < 6; i++)
		{
			if (i <= speedLevel) {
				barSegment->Draw(1, cam_x + 10 + 51 + i * 8, cam_y + 184 + 4 + 6);
			}
		}

		auto p = sprites->Get(29);
		p->Draw(1, cam_x + 10 + 99, cam_y + 184 + 4 + 6);
	}
}

string HUD::NumberToString(int num, int numOfChar)
{
	string numStr = to_string(num);
	int delta = numOfChar - numStr.length();
	for (int i = 0; i < delta; i++)
	{
		numStr = "0" + numStr;
	}
	return numStr;
}
