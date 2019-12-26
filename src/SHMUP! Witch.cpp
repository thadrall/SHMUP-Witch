#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "patherns.h"
#include <array>
#include <cstdint>
#include <algorithm>


// Override base class with your custom functionality
class SHUMP : public olc::PixelGameEngine
{
public:
	SHUMP()
	{
		sAppName = "SHMUP! Witch";
	}

	std::list<sEnemyDefinition> listSpawns;
	std::list<sEnemyDefinition> listSpawnsBkp;
	std::list<sEnemy> listEnemy;
	std::list<sBullet> listEnemyBullet;
	std::list<sBullet> listPlayerBullet;
	std::list<sBullet> listFragments;
	std::list<Player> listPlayer;

	bool bGame = true;
	bool bStart = true;

	int nHiScore = 0;

	Player Player1;

	void Explode(sEnemy& e) {
		for (int i = 0; i < ((int)sprEnemy[e.def.nSpriteID]->height); i++)
		{
			float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50;
			listFragments.push_back(
				{
					e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]),
						{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}
				});
		}
	}
	void Explode(olc::vf2d pos) {
		for (int i = 0; i < 1000; i++)
		{
			float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50;
			listFragments.push_back(
				{
					pos + GetMiddle(sprPlayer),
						{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}
				});
		}
	}


public:

	//==============================================================CREATE=========================================================

	bool OnUserCreate() override
	{
		bGame = true;
		bStart = true;

		// LOAD SPRITES
		sprPlayer = new olc::Sprite("art/2dPixWitch.png");
		sprEnemy[0] = new olc::Sprite("art/pumpkin.png");
		sprEnemy[1] = new olc::Sprite("art/ghost.png");
		sprEnemy[2] = new olc::Sprite("art/bad_witch.png");
		sprEnemy[3] = new olc::Sprite("art/moon.png");
		sprEnemy[4] = new olc::Sprite("art/bat.png");
		sprEnemyBullet[0] = new olc::Sprite("art/bullet3.png");
		sprEnemyBullet[1] = new olc::Sprite("art/bullet2.png");
		sprPlayerBullet = new olc::Sprite("art/bullet.png");
		sprHealth = new olc::Sprite("art/cauldron.png");
		sprShield = new olc::Sprite("art/shield.png");
		sprTree = new olc::Sprite("art/tree.png");
		sprSky = new olc::Sprite("art/sky3.png");
		sprMountains = new olc::Sprite("art/mountains2.png");
		sprGameOver = new olc::Sprite("art/gameover.png");
		sprStart = new olc::Sprite("art/start.png");

		
		// ==========================SCENE=============================

		// TREES
		for (int i = 0; i < aryTree.size(); i++)
		{
			auto& s = aryTree[i];
			s.x = (i * (float)ScreenWidth() / (aryTree.size() - 2) * 2.1f) + rand() % 10 * 3;
			s.y = ((float)ScreenHeight() - (float)ScreenHeight() * 0.20) + rand() % 10 * 2;
		};

		Player1.pos = { (float)ScreenWidth() / 2, (float)ScreenHeight() / 2 };

		for (auto& s : aryStars) s = { (float)(rand() % ScreenWidth()), (float)(rand() % ScreenHeight()) };



		// LEVEL
		listSpawns =
		{
			{200.0f, 0, 2, 0.5f, Move_None, Fire_Straight, 0, 200},
			{455.0f, 4, 1, 0.55f, Move_Bat, Fire_None, 1, 50},
			{450.0f, 4, 1, 0.45f, Move_Bat, Fire_None, 1, 50},
			{475.0f, 4, 1, 0.40f, Move_Bat, Fire_None, 1, 50},
			{460.0f, 4, 1, 0.65f, Move_Bat, Fire_None, 1, 50},
			{480.0f, 4, 1, 0.75f, Move_Bat, Fire_None, 1, 50},
			{490.0f, 4, 1, 0.50f, Move_Bat, Fire_None, 1, 50},
			{500.0f, 4, 1, 0.65f, Move_Bat, Fire_None, 1, 50},
			{870.0f, 1, 3, 0.25f, Move_Sinusoidal, Fire_Triple, 1, 500},
			{1220.0f, 2, 2, 0.75f, Move_Sinusoidal_Fast, Fire_Fast, 1, 750},
			{1680.0f, 2, 2, 0.5f, Move_Sinusoidal_Fast, Fire_Fast, 1, 750},
			{2040.0f, 3, 5, 0.5f, Move_None, Fire_Moon, 0, 1500}
		};

		listSpawnsBkp = listSpawns;


		return true;
	}

	//============================================================== UPDATE ==============================================================

	bool OnUserUpdate(float fElapsedTime) override
	{
	// ==================== Player ================

				//INPUT
		if (GetKey(olc::W).bHeld) Player1.pos.y -= Player1.fSpeed * fElapsedTime * 1.2f;
		if (GetKey(olc::S).bHeld) Player1.pos.y += Player1.fSpeed * fElapsedTime * 1.2f;
		if (GetKey(olc::A).bHeld) Player1.pos.x -= Player1.fSpeed * fElapsedTime;
		if (GetKey(olc::D).bHeld) Player1.pos.x += Player1.fSpeed * fElapsedTime;

		if (Player1.pos.x <= 0) Player1.pos.x = 0;
		if (Player1.pos.y <= 0) Player1.pos.y = 0;
		if (Player1.pos.x + (float)sprPlayer->width >= (float)ScreenWidth()) Player1.pos.x = (float)ScreenWidth() - (float)sprPlayer->width;
		if (Player1.pos.y + (float)sprPlayer->height >= (float)ScreenHeight()) Player1.pos.y = (float)ScreenHeight() - (float)sprPlayer->height;



				//Fire
		Player1.fGunReloadTimer += fElapsedTime * 0.5f;

		if (Player1.fGunReloadTimer >= Player1.fFireRate)
		{
			Player1.fGunReloadTimer = 0.0f;
			Player1.bCanFire = true;
		}

		if (GetKey(olc::SPACE).bHeld)
		{
			if (bStart) bStart = false;
			if (Player1.bCanFire)
			{
				sBullet b;
				b.pos = { Player1.pos.x + (float)sprPlayer->width, Player1.pos.y + (float)sprPlayer->height / 2 };
				b.vel = { 500.0f, 0.0f };
				listPlayerBullet.push_back(b);
				Player1.bCanFire = false;
			}
		}

		//spawn bullet
		for (auto& b : listPlayerBullet) {
			b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
			for (auto& e : listEnemy)
			{
				if (((b.pos + GetMiddle(sprPlayerBullet)) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() < (float)sprEnemy[e.def.nSpriteID]->width / 2)
				{
					b.remove = true;
					e.def.nEnemyLives--;
					if (e.def.nEnemyLives <= 0)
					{
						Player1.nScore += e.def.nScore;
						Explode(e);
					}
				}
			}
		}

				//add World vel

		if ((GetKey(olc::D).bHeld) && (Player1.pos.x > 150)) fWorldSpeed += 60 * fElapsedTime;
		if (fWorldSpeed >= 300) fWorldSpeed = 300;
		if ((GetKey(olc::A).bHeld) && (Player1.pos.x < 250)) fWorldSpeed -= 160 * fElapsedTime;
		if (fWorldSpeed <= 60) fWorldSpeed = 60;

		Player1.pos.x -= fWorldSpeed * fElapsedTime * 0.8;


	//============= World =========

		if(!bStart) dWorldPos += fWorldSpeed * fElapsedTime;

		//Sky
		vSkyPos.x -= fElapsedTime * 5;

		// Fragments
		for (auto& f : listFragments) f.pos += (f.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;

		//Remove Bullets
		listEnemyBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });
		listPlayerBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });
		listFragments.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });

	//=============== Enemies ==================

		//Spawn 
		while(!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime)
		{
			sEnemy e;
			e.def = listSpawns.front();
			e.pos = {
				(float)ScreenWidth() + sprEnemy[e.def.nSpriteID]->width,
				e.def.fOffset * (float)ScreenHeight() - sprEnemy[e.def.nSpriteID]->height
			};
			listSpawns.pop_front();
			listEnemy.push_back(e);
		}

		// Update 
		for (auto& e : listEnemy)
		{
			e.Update(fElapsedTime, fWorldSpeed, listEnemyBullet, Player1);
		};

		// Fire
		for (auto& b : listEnemyBullet) {
			b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
			if ((b.pos - Player1.pos).mag() < (float)sprPlayer->width / 2)
			{
				b.remove = true;
				Player1.nLives--;
				if (Player1.nLives <= 0)
				{
					Explode(Player1.pos);
					bGame = false;
				}
			}
		}

		

		//Remove
		listEnemy.remove_if([&](const sEnemy& e) {return (e.pos.x < 0.0f) || e.def.nEnemyLives <= 0; });


	//	================================= DRAW ============================

			Clear(olc::BLACK);

				//Sky
				DrawSprite(vSkyPos.x, vSkyPos.y, sprSky);
				//Stars
				for (size_t i = 0; i < aryStars.size(); i++)
				{
					auto& s = aryStars[i];

					s.x -= fWorldSpeed * fElapsedTime * ((i < 270) ? 0.2f : 0.3f);
					Draw(s.x, s.y, ((i < 270) ? olc::DARK_GREY : olc::WHITE));
					{

					}
					if (s.x < 0)
					{
						s = { (float)(ScreenWidth()), (float)(rand() % ScreenHeight()) };
					}
				}


				//Sprites
				SetPixelMode(olc::Pixel::MASK);

				DrawSprite((vSkyPos.x * 8), ((float)ScreenHeight() - (float)ScreenHeight() * 1), sprMountains);

				//Fragments
				for (auto& f : listFragments) Draw(f.pos.x, f.pos.y, (!bGame) ? olc::GREEN : olc::MAGENTA);

				//Enemy
				for (auto& e : listEnemy) DrawSprite(e.pos.x, e.pos.y, sprEnemy[e.def.nSpriteID]);

				//Player
				if (bGame && !bStart)  DrawSprite(Player1.pos.x, Player1.pos.y, sprPlayer) ;

				//Bullets
				for (auto& b : listEnemyBullet) DrawSprite(b.pos.x, b.pos.y, sprEnemyBullet[b.nBulletTypeID]);
				for (auto& b : listPlayerBullet) DrawSprite(b.pos.x, b.pos.y, sprPlayerBullet);

				//Front Scene
				for (size_t i = 0; i < aryTree.size(); i++)
				{
					auto& s = aryTree[i];
					if (i < aryTree.size()/2) s.x -= (fWorldSpeed * fElapsedTime * 1.7f);
					else s.x -= (fWorldSpeed * fElapsedTime * 1.9f);
					DrawSprite(s.x, s.y, sprTree);
					if (s.x < -80)
					{
						s.x = { (float)ScreenWidth()};
					}
				}
				
				m += fElapsedTime * 5;
				float fMagic = (cosf(m) + 1) * 2;
				if (bStart)  DrawString((float)ScreenWidth() *0.375f, (float)ScreenHeight() * 0.6f, "PRESS SPACE TO START...", olc::Pixel(fMagic * 40.0f + 80, fMagic * 40.0f + 80, fMagic * 40.0f + 80));

				//UI
				DrawString((float)ScreenWidth() * 0.02f, (float)ScreenHeight() * 0.03f, "LIVES:");
				for (int i = 0; i < (int)Player1.nLives; i++) DrawSprite(((float)ScreenWidth() * 0.1f) + (24 * i + 1), (float)ScreenHeight() * 0.002f, sprHealth);
				auto sScore = std::to_string(Player1.nScore);
				DrawString((float)ScreenWidth()*0.8f, (float)ScreenHeight()*0.03f, "SCORE:");
				DrawString((float)ScreenWidth() * 0.88f, (float)ScreenHeight() * 0.03f, sScore);
				nHiScore = (Player1.nScore > nHiScore) ? Player1.nScore : nHiScore;
				sScore = std::to_string(nHiScore);
				DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.95f, "HI SCORE:", olc::DARK_GREY);
				DrawString((float)ScreenWidth() * 0.55f, (float)ScreenHeight() * 0.95f, sScore, olc::DARK_GREY);

				SetPixelMode(olc::Pixel::NORMAL);

				// ====WIN===
				if (listSpawns.empty() && listEnemy.empty())
				{
					fWorldSpeed = 10.0f;
					fCounter += fElapsedTime;
					if (fCounter > 3.5f)    DrawString((float)ScreenWidth() * 0.42f, (float)ScreenHeight() * 0.42f, "YOU WIN!", olc::WHITE, 2U);
					if (fCounter > 10)
					{
						dWorldPos = 0.0f;
						listEnemy.clear();
						listEnemyBullet.clear();
						listPlayerBullet.clear();
						listFragments.clear();
						listSpawns = listSpawnsBkp;
						Player1.nLives = 3;
						bGame = true;
						bStart = true;
						vSkyPos.x = 0;
						fCounter = 0.0f;
						Player1.nScore = 0;
					}
				}
				//  ===LOSE===
				if (!bGame)
				{
					fWorldSpeed = 10.0f;
					fCounter += fElapsedTime;
					if(fCounter > 1.5f)    DrawString((float)ScreenWidth() * 0.385f, (float)ScreenHeight() * 0.42f, "GAME OVER", olc::WHITE, 2U);
					if (fCounter > 5) 
					{
						dWorldPos = 0.0f;
						listEnemy.clear();
						listEnemyBullet.clear();
						listPlayerBullet.clear();
						listFragments.clear();
						listSpawns = listSpawnsBkp;
						Player1.nLives = 3;
						bGame = true;
						bStart = true;
						vSkyPos.x = 0;
						fCounter = 0.0f;
						Player1.nScore = 0;
					}
				}
				
				//if (GetMouse(0).bHeld) DrawLine(vPlayerPos + GetMiddle(sprPlayer), PGE::vf2d((float)GetMouseX(), (float)GetMouseY()),PGE::GREEN);
		return true;
	}
};
int main()
{
	SHUMP demo;
	if (demo.Construct(580, 280, 4, 4))
		demo.Start();
	return 0;
}