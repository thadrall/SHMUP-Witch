#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "patherns.h"
#include <array>
#include <cstdint>
#include <algorithm>
#include <iomanip>



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
	std::list<sBullet> listFragments;
	std::list<sBullet> listFragmentsPlayer1;
	std::list<sBullet> listFragmentsPlayer2;
	std::list<Player> listPlayers;


	bool bGame = true;
	bool bStart = true;
	bool bPause = false;
	bool bPlayer1 = false;
	bool bPlayer2 = false;

	//MENU FLAGS
	bool bResume = false;
	bool bRestart = false;
	bool bInfo = false;
	bool bQuit = false;
	bool bInfoPanel = false;

	int nHiScore = 0;
	float fBlinkCounter;
	int nScoreBuffor;

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
	void Explode(Player p) {
		for (int i = 0; i < 1000; i++)
		{
			float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50;
			if (p.nPlayerID == 1)
			{
				listFragmentsPlayer1.push_back(
					{
						p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]),
							{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}
					});
			}
			if (p.nPlayerID == 2)
			{
				listFragmentsPlayer2.push_back(
					{
						p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]),
							{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}
					});
			}
		}
	}

	void restart()
	{
		dWorldPos = 0.0f;
		listPlayers.clear();
		listEnemy.clear();
		listEnemyBullet.clear();
		listFragments.clear();
		listFragmentsPlayer1.clear();
		listFragmentsPlayer2.clear();
		listSpawns = listSpawnsBkp;
		bGame = true;
		bStart = true;
		bPlayer1 = false;
		bPlayer2 = false;
		vSkyPos.x = 0;
		fCounter = 0.0f;
		fWorldSpeed = 90.0f;
		nScoreBuffor = 0.0f;
	}


	//==============================================================CREATE=========================================================

	bool OnUserCreate() override
	{
		bGame = true;
		bStart = true;
		bPause = false;

		// LOAD SPRITES
		sprPlayer[0] = new olc::Sprite("art/player1.png");
		sprPlayer[1] = new olc::Sprite("art/Player2.png");
		sprEnemy[0] = new olc::Sprite("art/pumpkin.png");
		sprEnemy[1] = new olc::Sprite("art/ghost.png");
		sprEnemy[2] = new olc::Sprite("art/bad_witch.png");
		sprEnemy[3] = new olc::Sprite("art/moon.png");
		sprEnemy[4] = new olc::Sprite("art/bat.png");
		sprEnemyBullet[0] = new olc::Sprite("art/bullet3.png");
		sprEnemyBullet[1] = new olc::Sprite("art/bullet2.png");
		sprPlayerBullet[0] = new olc::Sprite("art/player1bullet.png");
		sprPlayerBullet[1] = new olc::Sprite("art/player2bullet.png");
		sprHealth[0] = new olc::Sprite("art/health1.png");
		sprHealth[1] = new olc::Sprite("art/health2.png");
		sprShield[0] = new olc::Sprite("art/shield.png");
		sprShield[1] = new olc::Sprite("art/shield2.png");
		sprTree = new olc::Sprite("art/tree.png");
		sprSky[0] = new olc::Sprite("art/sky1.png");
		sprSky[1] = new olc::Sprite("art/sky2.png");
		sprMountains[0] = new olc::Sprite("art/mountains.png");
		sprMountains[1] = new olc::Sprite("art/mountains.png");
		sprGameOver = new olc::Sprite("art/gameover.png");
		sprStart = new olc::Sprite("art/start.png");

		
		// ==========================SCENE=============================

		//SKY
		for (int i = 0; i < arySky.size(); i++)
		{
			auto& s = arySky[i];
			s.x = 514 * i;
			s.y =  0;
		}

		//MOUNTAINS
		for (int i = 0; i < aryMountains.size(); i++)
		{
			auto& m = aryMountains[i];
			m.x = 1600 * i;
			m.y = 0;
		}

		// TREES
		for (int i = 0; i < aryTree.size(); i++)
		{
			auto& t = aryTree[i];
			t.x = (i * (float)ScreenWidth() / (aryTree.size() - 2) * 2.1f) + rand() % 10 * 3;
			t.y = ((float)ScreenHeight() - (float)ScreenHeight() * 0.20) + rand() % 10 * 2;
		};

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
			{1020.0f, 1, 3, 0.7f, Move_Sinusoidal, Fire_Triple, 1, 500},
			{1220.0f, 2, 2, 0.75f, Move_Sinusoidal_Fast, Fire_Fast, 1, 750},
			{1300.0f, 0, 2, 0.7f, Move_None, Fire_Straight, 0, 200},
			{1300.0f, 0, 2, 0.3f, Move_None, Fire_Straight, 0, 200},
			{1500.0f, 0, 2, 0.5f, Move_None, Fire_Straight, 0, 200},
			{1680.0f, 2, 2, 0.5f, Move_Sinusoidal_Fast, Fire_Fast, 1, 750},
			{1655.0f, 4, 1, 0.65f, Move_Bat, Fire_None, 1, 50},
			{1650.0f, 4, 1, 0.45f, Move_Bat, Fire_None, 1, 50},
			{1675.0f, 4, 1, 0.50f, Move_Bat, Fire_None, 1, 50},
			{1660.0f, 4, 1, 0.55f, Move_Bat, Fire_None, 1, 50},
			{1680.0f, 4, 1, 0.85f, Move_Bat, Fire_None, 1, 50},
			{1690.0f, 4, 1, 0.60f, Move_Bat, Fire_None, 1, 50},
			{1700.0f, 4, 1, 0.55f, Move_Bat, Fire_None, 1, 50},
			{2040.0f, 3, 5, 0.5f, Move_None, Fire_Moon, 0, 1500}
		};

		listSpawnsBkp = listSpawns;


		return true;
	}

	//============================================================== UPDATE ==============================================================

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::ESCAPE).bPressed && bGame)
			if (!bPause)
			{
				bPause = true;
				bResume = true;
			}
			else if(!bInfoPanel)
			{ 
				bPause = false;
				bResume = false;
				bRestart = false;
				bInfo = false;
				bQuit = false;
			}
			else
			{
				bResume = true;
				bInfoPanel = false;
			}

		if (!bPause) {
			// ==================== Player ================

				//CREATE
				/*
				if (GetKey(olc::ENTER).bPressed && bGame && listPlayers.size() < 2)
				{
					if (bStart) bStart = false;
					if (!bPlayer1)
					{
						Player* player = new Player(1);
						listPlayers.push_back(*player);
						bPlayer1 = true;
					}
					else
					{
						Player* player = new Player(2);
						listPlayers.push_back(*player);
						bPlayer2 = true;
					}
				}
				*/

			if (GetKey(olc::SPACE).bPressed && !bPlayer1 && bGame)
			{
				Player* player = new Player(1);
				listPlayers.push_back(*player);
				bPlayer1 = true;
				if (bStart) bStart = false;
			}

			if (GetKey(olc::NP0).bPressed && !bPlayer2 && bGame)
			{
				Player* player = new Player(2);
				listPlayers.push_back(*player);
				bPlayer2 = true;
				if (bStart) bStart = false;
			}

			//PLAYER UPDATE + INPUT
			for (auto& p : listPlayers)
			{

				//INPUT

		//Player1
				if (p.nPlayerID == 1)
				{
					if (GetKey(olc::W).bHeld) p.pos.y -= p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::S).bHeld) p.pos.y += p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::A).bHeld) p.pos.x -= p.fSpeed * fElapsedTime;
					if (GetKey(olc::D).bHeld) p.pos.x += p.fSpeed * fElapsedTime;
					p.bIsFire = (GetKey(olc::SPACE).bHeld) ? 1 : 0;
				}
				//Player2
				if (p.nPlayerID == 2)
				{
					if (GetKey(olc::UP).bHeld) p.pos.y -= p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::DOWN).bHeld) p.pos.y += p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::LEFT).bHeld) p.pos.x -= p.fSpeed * fElapsedTime;
					if (GetKey(olc::RIGHT).bHeld) p.pos.x += p.fSpeed * fElapsedTime;
					p.bIsFire = (GetKey(olc::NP0).bHeld) ? 1 : 0;
				}

				//WORLD SPEED
				p.pos.x -= fWorldSpeed * fElapsedTime * 0.8;

				//CLAMP
				if (p.pos.x <= 0) p.pos.x = 0;
				if (p.pos.y <= 0) p.pos.y = 0;
				if (p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width >= (float)ScreenWidth()) p.pos.x = (float)ScreenWidth() - (float)sprPlayer[p.nPlayerID - 1]->width;
				if (p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height >= (float)ScreenHeight()) p.pos.y = (float)ScreenHeight() - (float)sprPlayer[p.nPlayerID - 1]->height;


				//FIRE
				if (p.bIsFire)
				{
					if (p.bCanFire)
					{
						sBullet b;
						b.pos = { p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width, p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height / 2 };
						b.vel = { 500.0f, 0.0f };
						b.nBulletPlayerID = p.nPlayerID;
						p.listPlayerBullet.push_back(b);
						p.bCanFire = false;
					}
				}

				//spawn bullet
				for (auto& b : p.listPlayerBullet)
				{
					b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
					for (auto& e : listEnemy)
					{
						if (((b.pos + GetMiddle(sprPlayerBullet[b.nBulletPlayerID + 1])) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() < (float)sprEnemy[e.def.nSpriteID]->width / 1.85f)
						{
							b.remove = true;
							e.def.nEnemyLives--;
							if (e.def.nEnemyLives <= 0)
							{
								Explode(e);
								p.nScore += e.def.nScore;
								nHiScore = (p.nScore > nHiScore) ? p.nScore : nHiScore;
							}
						}
					}
				}
				//REMOVE BULLETS
				p.listPlayerBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });

				//COUNTERS
				p.fGunReloadTimer += fElapsedTime * 0.5f;
				if (p.fGunReloadTimer >= p.fFireRate)
				{
					p.fGunReloadTimer -= p.fFireRate;
					p.bCanFire = true;
				}

				p.fProtectionTimer += fElapsedTime;
				if (p.fProtectionTimer >= p.fCooldown)
				{
					p.bIsProtected = false;
				}
			}
			//REMOVE
			listPlayers.remove_if([&](const Player& p) {return p.bDead; });


	//============= World =========

			if (!bStart) dWorldPos += fWorldSpeed * fElapsedTime;

			//Sky
			vSkyPos.x -= fElapsedTime * 5;

			// Fragments
			for (auto& f : listFragments) f.pos += (f.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
			for (auto& f : listFragmentsPlayer1) f.pos += (f.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
			for (auto& f : listFragmentsPlayer2) f.pos += (f.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;


			listFragments.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });
			listFragmentsPlayer1.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });
			listFragmentsPlayer2.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });

			//=============== Enemies ==================

				//Spawn 
			while (!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime)
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
				olc::vf2d vNearestPlayer = { -1000,200 };
				for (auto& p : listPlayers)
				{
					if ((p.pos - e.pos).mag() < (vNearestPlayer - e.pos).mag()) vNearestPlayer = p.pos;

				}
				e.Update(fElapsedTime, fWorldSpeed, listEnemyBullet, vNearestPlayer);
				for (auto& p : listPlayers)
				{
					if (((e.pos + GetMiddle(sprEnemy[e.def.nSpriteID])) - (p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]))).mag() < (float)sprPlayer[p.nPlayerID - 1]->width / 1.95f && !p.bIsProtected)
					{
						p.nLives--;
						if (p.nLives <= 0)
						{
							Explode(p);
							if (p.nPlayerID == 1) bPlayer1 = false;
							if (p.nPlayerID == 2) bPlayer2 = false;
							p.bDead = true;
							nScoreBuffor = p.nScore;
						}
						p.bIsProtected = true;
						p.fProtectionTimer = 0.0f;
					}
				}
			};

			// Fire
			for (auto& b : listEnemyBullet)
			{
				b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
				for (auto& p : listPlayers)
				{
					if (((b.pos + GetMiddle(sprEnemyBullet[b.nBulletTypeID])) - (p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]))).mag() < (float)sprPlayer[p.nPlayerID - 1]->width / 1.95f && !p.bIsProtected)
					{
						b.remove = true;
						p.nLives--;
						if (p.nLives <= 0)
						{
							Explode(p);
							if (p.nPlayerID == 1) bPlayer1 = false;
							if (p.nPlayerID == 2) bPlayer2 = false;
							p.bDead = true;
							nScoreBuffor = p.nScore;
						}
						p.bIsProtected = true;
						p.fProtectionTimer = 0.0f;
					}
				}

			}

			//Remove Bullets
			listEnemyBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.remove; });
			//Remove
			listEnemy.remove_if([&](const sEnemy& e) {return (e.pos.x < 0.0f) || e.def.nEnemyLives <= 0; });
		}
		if(bPause)
		{
			if (bResume)
			{
				if (GetKey(olc::ENTER).bPressed) { bPause = false; bResume = false; }
				if (GetKey(olc::UP).bPressed) { bQuit = true; bResume = false; }
				if (GetKey(olc::DOWN).bPressed) { bRestart = true; bResume = false; }
			}
			else if (bRestart)
			{
				if (GetKey(olc::ENTER).bPressed) { restart(); bRestart = false; bPause = false; }
				if (GetKey(olc::UP).bPressed) { bResume = true; bRestart = false; }
				if (GetKey(olc::DOWN).bPressed) { bInfo = true; bRestart = false; }
			}
			else if (bInfo)
			{
				if (GetKey(olc::ENTER).bPressed) { bInfoPanel = true; bInfo = false; }
				if (GetKey(olc::UP).bPressed) { bRestart = true; bInfo = false; }
				if (GetKey(olc::DOWN).bPressed) { bQuit = true; bInfo = false; }
			}
			else if (bQuit)
			{
				if (GetKey(olc::ENTER).bPressed) { exit(0); }
				if (GetKey(olc::UP).bPressed) { bInfo = true; bQuit = false; }
				if (GetKey(olc::DOWN).bPressed) { bResume = true; bQuit = false; }
			}
		}
	//	================================= DRAW ============================

			Clear(olc::BLACK);
		
			//Blink
			fBlinkCounter += fElapsedTime * 5;
			float fMagic = (cosf(fBlinkCounter) + 1) * 2;
			olc::Pixel magicPixel = olc::Pixel(fMagic * 40.0f + 60, fMagic * 40.0f + 60, fMagic * 40.0f + 60);

				//Sky
				for (size_t i = 0; i < arySky.size(); i++)
				{
					auto& s = arySky[i];

					s.x -= fWorldSpeed * fElapsedTime * 0.3f;
					DrawSprite(s.x, s.y, sprSky[i%2]);
					if (s.x + sprSky[i%2]->width < 0) s.x = sprSky[i%2]->width * 3;
				}

				//Stars
				for (size_t i = 0; i < aryStars.size(); i++)
				{
					auto& s = aryStars[i];

					s.x -= fWorldSpeed * fElapsedTime * ((i < 270) ? 0.35f : 0.45f);
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

				for (int i = 0; i < aryMountains.size(); i++)
				{
					auto& m = aryMountains[i];

					m.x -= fWorldSpeed * fElapsedTime;
					DrawSprite(m.x, m.y, sprMountains[i % 2]);
					if (m.x + sprMountains[i]->width < 0) m.x = sprMountains[i]->width;
				}
				//Fragments
				for (auto& f : listFragments) Draw(f.pos.x, f.pos.y, olc::MAGENTA);
				for (auto& f : listFragmentsPlayer1) Draw(f.pos.x, f.pos.y, olc::GREEN);
				for (auto& f : listFragmentsPlayer2) Draw(f.pos.x, f.pos.y, olc::CYAN);

				//Enemy
				for (auto& e : listEnemy) DrawSprite(e.pos.x, e.pos.y, sprEnemy[e.def.nSpriteID]);
				for (auto& b : listEnemyBullet) DrawSprite(b.pos.x, b.pos.y, sprEnemyBullet[b.nBulletTypeID]);

				//Player
				for (auto& p : listPlayers)
				{
					DrawSprite(p.pos.x, p.pos.y, sprPlayer[p.nPlayerID - 1]);
					for (auto& b : p.listPlayerBullet) DrawSprite(b.pos.x, b.pos.y, sprPlayerBullet[p.nPlayerID - 1]);
					if (p.bIsProtected) DrawSprite(p.pos.x -2, p.pos.y -2, sprShield[p.nPlayerID-1]);
				}

				//Front Scene
				for (size_t i = 0; i < aryTree.size(); i++)
				{
					auto& t = aryTree[i];
					if (i < aryTree.size()/2) t.x -= (fWorldSpeed * fElapsedTime * 1.7f);
					else t.x -= (fWorldSpeed * fElapsedTime * 1.9f);
					DrawSprite(t.x, t.y, sprTree);
					if (t.x < -80)
					{
						t.x = { (float)ScreenWidth()};
					}
				}

				//UI
				for (auto& p : listPlayers) {
					float offset = (p.nPlayerID - 1) ? 0.80f : 0.02f;
					std::ostringstream ss;
					ss << std::setw(10) << std::setfill('0') << p.nScore;
					DrawString((float)ScreenWidth() * offset + 10, (float)ScreenHeight() * 0.035f, (ss.str() + "\n\nP" + std::to_string(p.nPlayerID) + ":"));
					for (int i = 0; i < (int)p.nLives; i++) DrawSprite((float)ScreenWidth() * offset + 40 + sprHealth[p.nPlayerID-1]->width*i, (float)ScreenHeight() * 0.07f, sprHealth[p.nPlayerID - 1]);
				}

				SetPixelMode(olc::Pixel::NORMAL);

				//JOIN PLAYER 
				if (!bPlayer1 && !bStart && bGame)  DrawString((float)ScreenWidth() * 0.05f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
				if (!bPlayer2 && !bStart && bGame)  DrawString((float)ScreenWidth() * 0.8f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
				if (!bPlayer1 && !bStart)  DrawString((float)ScreenWidth() * 0.05f, (float)ScreenHeight() * 0.030f, "SCORE: " + std::to_string(nScoreBuffor));
				if (!bPlayer2 && !bStart)  DrawString((float)ScreenWidth() * 0.8f, (float)ScreenHeight() * 0.030f, "SCORE: " + std::to_string(nScoreBuffor));
				//START
				if (bStart && !bPause)
				{
					DrawString((float)ScreenWidth() * 0.42f, (float)ScreenHeight() * 0.25f, "SHMUP!", olc::WHITE, 3U);
					DrawString((float)ScreenWidth() * 0.37f, (float)ScreenHeight() * 0.38f, "WITCH", olc::WHITE, 5U);
					DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.7f, "PRESS FIRE TO START...", olc::Pixel(fMagic * 40.0f + 80, fMagic * 40.0f + 80, fMagic * 40.0f + 80));
				}
				//HI SCORE
				auto sHiScore = std::to_string(nHiScore);
				DrawString((float)ScreenWidth() * 0.44f, (float)ScreenHeight() * 0.95f, "HI SCORE: " + std::to_string(nHiScore), olc::DARK_GREY);


				// ====WIN===
				if (listSpawns.empty() && listEnemy.empty() && bGame)
				{
					
					fCounter += fElapsedTime;
					if (fCounter > 3.5f)
					{
						fWorldSpeed = 10.0f;
						if(!bPause)DrawString((float)ScreenWidth() * 0.42f, (float)ScreenHeight() * 0.42f, "YOU WIN!", olc::WHITE, 2U);
					}
				}
				//  ===LOSE===
				if (listPlayers.empty() && !bStart)
				{
					bGame = false;
					fWorldSpeed = 10.0f;
					fCounter += fElapsedTime;
					if(fCounter > 1.5f)    DrawString((float)ScreenWidth() * 0.385f, (float)ScreenHeight() * 0.42f, "GAME OVER", olc::WHITE, 2U);
					if (fCounter > 5) restart();
				}
				
				if (bPause)
				{
					if (!bInfoPanel) 
					{
						if(!bStart)DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.28f, "PAUSE!", olc::WHITE, 3U);

						DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.42f, "Resume", (bResume) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.52f, "Restart", (bRestart) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.62f, "Controls", (bInfo) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.72f, "Quit", (bQuit) ? magicPixel : olc::DARK_GREY, 2U);
					}
					if (bInfoPanel)
					{
						DrawString((float)ScreenWidth() * 0.35f, (float)ScreenHeight() * 0.18f, "CONTROLS", olc::DARK_GREY, 3U);
						DrawString((float)ScreenWidth() * 0.13f, (float)ScreenHeight() * 0.34f, "PLAYER1", olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.08f, (float)ScreenHeight() * 0.44f, "W, S, A, D = Movement \n\nSPACE = Fire", olc::DARK_GREY);

						DrawString((float)ScreenWidth() * 0.63f, (float)ScreenHeight() * 0.34f, "PLAYER2", olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.58f, (float)ScreenHeight() * 0.44f, "ARROW KEYS = Movement \n\nNUM 0 = Fire", olc::DARK_GREY);
					}
				}
				//if (GetMouse(0).bHeld) DrawLine(vPlayerPos + GetMiddle(sprPlayer), PGE::vf2d((float)GetMouseX(), (float)GetMouseY()),PGE::GREEN);
		return true;
	}
};
int main()
{
	SHUMP demo;
	if (demo.Construct(640, 320, 3, 3, true))
		demo.Start();
	return 0;
}