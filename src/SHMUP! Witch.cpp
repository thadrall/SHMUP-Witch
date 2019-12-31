#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "patherns.h"
#include <array>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib")


class SHUMP : public olc::PixelGameEngine
{
public:
	SHUMP()
	{
		sAppName = "SHMUP! Witch";
	}

	//LISTS
	std::list<sEnemyDefinition> listSpawns;
	std::list<sEnemyDefinition> listSpawnsBkp;
	std::list<sEnemy> listEnemy;
	std::list<sBullet> listEnemyBullet;
	std::list<sBullet> listFragments;
	std::list<sBullet> listFragmentsPlayer1;
	std::list<sBullet> listFragmentsPlayer2;
	std::list<Player> listPlayers;
	std::list<sSplash> listSplash;
	std::list<sDrops> listDrops;


	//SOUND
	irrklang::ISoundEngine* SoundEngine;

	//GAME FLAGS
	bool bGame = true;
	bool bStart = true;
	bool bPause = false;
	bool bPlayer1 = false;
	bool bPlayer2 = false;
	bool bBoss = false;

	//MENU FLAGS
	bool bResume = false;
	bool bRestart = false;
	bool bInfo = false;
	bool bQuit = false;
	bool bInfoPanel = false;

	int nHiScore = 0;
	float fBlinkCounter;
	float fBossHealth;

	//BOOSTS
	sBoost boostSpeed;

	//ANIMATIONS

	sAnimation aSpecialP1;
	sAnimation aSpecialP2;

	//FUNCTIONS

	void Explode(sEnemy& e) {
		for (int i = 0; i < ((int)sprEnemy[e.def.nSpriteID]->height) * 2; i++)
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
	void Explode(Player p, olc::vf2d pos, int f, olc::vf2d offset = { 0,0 }) {
		for (int i = 0; i < f; i++)
		{
			float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50;
			if (p.nPlayerID == 1)
			{
				listFragmentsPlayer1.push_back(
					{
						pos + offset,
							{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}
					});
			}
			if (p.nPlayerID == 2)
			{
				listFragmentsPlayer2.push_back(
					{
						pos + offset,
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
		bBoss = false;
		vSkyPos.x = 0;
		fCounter = 0.0f;
		fWorldSpeed = 90.0f;
		SoundEngine->stopAllSounds();
		SoundEngine->play2D("art/sounds/slayer_south_of_heaven_8_bit.mp3", GL_TRUE);
	}
	void kill(sEnemy& e, Player& p)
	{
		Explode(e);
		p.nScore += e.def.nScore;
		nHiScore = (p.nScore > nHiScore) ? p.nScore : nHiScore;
		SoundEngine->play2D("art/sounds/explode.wav", GL_FALSE);
		int nLoot = (rand() % 100);
		if (e.def.nDropRate > nLoot)
		{
			sDrops d;
			d.pos = e.pos;
			d.nID = (rand() % 2);
			listDrops.push_back(d);
		}
	}

	//==============================================================CREATE=========================================================

	bool OnUserCreate() override
	{

		bGame = true;
		bStart = true;
		bPause = false;
		vScreenSize = { (float)ScreenWidth(), (float)ScreenHeight() };

		//BOOSTS
		boostSpeed.nBoostID = 1;
		boostSpeed.offset = { -23,19 };
		boostSpeed.fDuration = 8.0f;
		boostSpeed.fBonusValue = 100.0f;
		boostSpeed.fBoostCounter = 0.0f;
		boostSpeed.fManaCost = 15.0f;

		//SOUND
		SoundEngine = irrklang::createIrrKlangDevice();
		SoundEngine->play2D("art/sounds/slayer_south_of_heaven_8_bit.mp3", GL_TRUE);

		// LOAD SPRITES
		sprPlayer[0] = new olc::Sprite("art/player1.png");
		sprPlayer[1] = new olc::Sprite("art/Player2.png");
		sprPlayerAtack[0] = new olc::Sprite("art/player1atack.png");
		sprPlayerAtack[1] = new olc::Sprite("art/player2atack.png");

		sprEnemy[0] = new olc::Sprite("art/pumpkin.png");
		sprEnemy[1] = new olc::Sprite("art/ghost.png");
		sprEnemy[2] = new olc::Sprite("art/bad_witch.png");
		sprEnemy[3] = new olc::Sprite("art/moon.png");
		sprEnemy[4] = new olc::Sprite("art/bat.png");

		sprEnemyBullet[0] = new olc::Sprite("art/bullet3.png");
		sprEnemyBullet[1] = new olc::Sprite("art/bullet2.png");

		sprPlayerBullet[0] = new olc::Sprite("art/player1bullet.png");
		sprPlayerBullet[1] = new olc::Sprite("art/player2bullet.png");
		sprPlayerBullet[2] = new olc::Sprite("art/wave1.png");
		sprPlayerBullet[3] = new olc::Sprite("art/wave2.png");

		sprHealth[0] = new olc::Sprite("art/health1.png");
		sprHealth[1] = new olc::Sprite("art/health2.png");

		sprShield[0] = new olc::Sprite("art/shield.png");
		sprShield[1] = new olc::Sprite("art/shield2.png");

		sprTree = new olc::Sprite("art/tree.png");

		sprSky[0] = new olc::Sprite("art/sky1.png");
		sprSky[1] = new olc::Sprite("art/sky2.png");

		sprMountains[0] = new olc::Sprite("art/mountains.png");
		sprMountains[1] = new olc::Sprite("art/mountains.png");

		sprSplash[0] = new olc::Sprite("art/splash1.png");
		sprSplash[1] = new olc::Sprite("art/splash2.png");

		sprPowerUps[0] = new olc::Sprite("art/scroll.png");
		sprPowerUps[1] = new olc::Sprite("art/scroll2.png");

		sprManaFrame[0] = new olc::Sprite("art/manaP1d.png");
		sprManaFrame[1] = new olc::Sprite("art/manaP2d.png");

		sprBoost[0] = new olc::Sprite("art/boost1.png");
		sprBoost[1] = new olc::Sprite("art/boost2.png");
		
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell0.png"));
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell1.png"));
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell2.png"));

		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning0.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning1.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning2.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning3.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning4.png"));
		
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
		{	//TIME			OFFSET		SPRITE		BOOS?		LIVES		MOVE				FIRE_PATERN			DROP_RATE			SCORE
			{200.0f,		0.5f,		0,			0,			2,			Move_None,			Fire_Straight,		100,			200		},
			{455.0f,		0.55f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{450.0f,		0.45f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{475.0f,		0.40f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{460.0f,		0.65f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{480.0f,		0.75f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{490.0f,		0.50f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{500.0f,		0.65f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{870.0f,		0.25f,		1,			0,			3,			Move_Sin,			Fire_Triple,		20,				500		},
			{1020.0f,		0.7f,		1,			0,			3,			Move_Sin,			Fire_Triple,		20,				500		},
			{1220.0f,		0.75f,		2,			0,			2,			Move_Sin_Fast,		Fire_Fast,			20,				750		},
			{1300.0f,		0.7f,		0,			0,			2,			Move_None,			Fire_Straight,		15,				200		},
			{1300.0f,		0.3f,		0,			0,			2,			Move_None,			Fire_Straight,		15,				200		},
			{1500.0f,		0.5f,		0,			0,			2,			Move_None,			Fire_Straight,		15,				200		},
			{1680.0f,		0.5f,		2,			0,			2,			Move_Sin_Fast,		Fire_Fast,			15,				750		},
			{1655.0f,		0.65f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1650.0f,		0.45f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1675.0f,		0.50f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1660.0f,		0.55f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1680.0f,		0.85f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1690.0f,		0.60f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{1700.0f,		0.55f,		4,			0,			1,			Move_Bat,			Fire_None,			5,				50		},
			{2000.0f,		0.5f,		3,			1,			50,			Move_Moon,			Fire_Moon,			25,				1500	}
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
				SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE);
			}
			else if(!bInfoPanel)
			{ 
				bPause = false;
				bResume = false;
				bRestart = false;
				bInfo = false;
				bQuit = false;
				SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE);
			}
			else
			{
				bResume = true;
				bInfoPanel = false;
				SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE);
			}

		if (!bPause) {
			// ==================== Player ================

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
					/*
					p.bSpeciall = (GetKey(olc::V).bHeld) ? 1 : 0;
					p.bBoost = (GetKey(olc::B).bHeld) ? 1 : 0;
				*/
				}
				//Player2
				if (p.nPlayerID == 2)
				{
					if (GetKey(olc::UP).bHeld) p.pos.y -= p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::DOWN).bHeld) p.pos.y += p.fSpeed * fElapsedTime * 1.2f;
					if (GetKey(olc::LEFT).bHeld) p.pos.x -= p.fSpeed * fElapsedTime;
					if (GetKey(olc::RIGHT).bHeld) p.pos.x += p.fSpeed * fElapsedTime;
					p.bIsFire = (GetKey(olc::NP0).bHeld) ? 1 : 0;
					p.bSpeciall = (GetKey(olc::NP_ADD).bHeld) ? 1 : 0;
					p.bBoost = (GetKey(olc::NP_SUB).bHeld) ? 1 : 0;
				}

				//WORLD SPEED
				p.pos.x -= fWorldSpeed * fElapsedTime * 0.8;

				//CLAMP
				if (p.pos.x <= 0) p.pos.x = 0;
				if (p.pos.y <= 0) p.pos.y = 0;
				if (p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width >= (float)ScreenWidth()) p.pos.x = (float)ScreenWidth() - (float)sprPlayer[p.nPlayerID - 1]->width;
				if (p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height >= (float)ScreenHeight()) p.pos.y = (float)ScreenHeight() - (float)sprPlayer[p.nPlayerID - 1]->height;

				//POWER UPS
				for (auto& d : listDrops)
				{
					if (((p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1])) - (d.pos + GetMiddle(sprPowerUps[d.nID]))).mag() <= sprPowerUps[d.nID]->width) 
					{
						SoundEngine->play2D("art/sounds/powerup.wav", GL_FALSE);
						d.bRemove = true;
						if (d.nID == 0) p.fFireRate -= 0.04f;
						if (d.nID == 1) p.fSpeed += 20.0f;
					}
				}

				//FIRE
				if (p.bIsFire && p.bCanFire)
				{
					sBullet b;
					b.pos = { p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width, p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height / 3 };
					b.vel = { 500.0f, 0.0f };
					b.nBulletTypeID = 0;
					p.listPlayerBullet.push_back(b);
					p.bCanFire = false;
					SoundEngine->play2D("art/sounds/spell.wav", GL_FALSE);
				}

				//spawn bullet
				for (auto& b : p.listPlayerBullet)
				{
					b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
					for (auto& e : listEnemy)
					{
						if (((b.pos + GetMiddle(sprPlayerBullet[b.nBulletTypeID])) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() <( (float)sprEnemy[e.def.nSpriteID]->width + (float)sprPlayerBullet[b.nBulletTypeID]->width) / 2.0f)
						{
							if(b.nBulletTypeID != 2)b.bRemove = true;
							e.def.nEnemyLives--;
							SoundEngine->play2D("art/sounds/gloop.wav", GL_FALSE);
							Explode(p, b.pos, 8);
							sSplash s;
							s.pos = b.pos;
							s.nID = p.nPlayerID;
							s.fCounter = 0.0f;
							listSplash.push_back(s);
							if (e.def.nEnemyLives <= 0)
							{
								kill(e, p);
							}
						}
					}
				}

				//REMOVE BULLETS
				p.listPlayerBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.bRemove; });

				//SPECIAL
				if (p.bSpeciall && p.fMana > p.fSpeciallManaCost && p.bSpeciallReady)
				{
					sSpell s;
					s.pos = { p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width, p.pos.y - 15 };
					s.vel = { 280.0f, 0.0f };
					p.listPlayerSpeciall.push_back(s);
					p.fMana -= p.fSpeciallManaCost;
					p.bSpeciallReady = false;
					if(p.nPlayerID == 1) SoundEngine->play2D("art/sounds/warp.wav", GL_FALSE);
					else SoundEngine->play2D("art/sounds/electricShock.wav", GL_FALSE);
				}
				for (auto& s : p.listPlayerSpeciall)
				{
					s.pos += (s.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
					s.fCounter += fElapsedTime;
					if (s.fCounter > 0.08f) {
						s.fCounter = 0.0f;
						s.bReady = true;
					}
					for (auto& e : listEnemy)
					{
						if (((s.pos + GetMiddle(aSpecialP1.playAnimation(fElapsedTime))) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() < ((float)sprEnemy[e.def.nSpriteID]->width + (float)aSpecialP1.playAnimation(fElapsedTime)->width) / 2.0f  && s.bReady )
						{
							e.def.nEnemyLives--;
							if (p.nPlayerID == 1)SoundEngine->play2D("art/sounds/zap.wav", GL_FALSE);
							else SoundEngine->play2D("art/sounds/zap.mp3", GL_FALSE);
							Explode(p, s.pos, 8);
							sSplash sp;
							sp.pos = e.pos;
							sp.nID = p.nPlayerID;
							sp.fCounter = 0.0f;
							listSplash.push_back(sp);
							s.bReady = false;
							if (e.def.nEnemyLives <= 0)
							{
								kill(e, p);
							}
						}
					}
				}
				p.listPlayerSpeciall.remove_if([&](const sSpell& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight(); });


				//BOOST

				if (p.bBoost && !p.bBoostActive)
				{
					if (p.fMana >= boostSpeed.fManaCost / 3) {
						p.listPlayerBoost.push_back(boostSpeed);
						p.fSpeed += boostSpeed.fBonusValue;
						p.bBoostActive = true;
					}
				}

				if (p.bBoostActive)
				{
					for (auto& b : p.listPlayerBoost)
					{
						p.fMana -= b.fManaCost * fElapsedTime;
						b.fCharge += fElapsedTime;
						if (b.fCharge > 0.3f) {
							b.fCharge = 0.0f;
							b.bReady = true;
						}
						for (auto& e : listEnemy)
						{
							if (((p.pos + b.offset) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() < ((float)sprEnemy[e.def.nSpriteID]->width + (float)sprBoost[0]->width) / 2.2f && b.bReady)
							{
								e.def.nEnemyLives--;
								SoundEngine->play2D("art/sounds/zap.wav", GL_FALSE);
								Explode(p, (p.pos + b.offset * 1.5f + GetMiddle(sprBoost[0])), 8);
								sSplash s;
								s.pos = (p.pos + b.offset * 1.5f);
								s.nID = p.nPlayerID;
								s.fCounter = 0.0f;
								listSplash.push_back(s);
								b.bReady = false;
								if (e.def.nEnemyLives <= 0)
								{
									kill(e, p);
								}
							}
						}

						//Explode(p, p.pos, 1, {-18, 30});
						b.fBoostCounter += fElapsedTime;
						if (b.fBoostCounter >= b.fDuration || p.fMana <= 1 || !p.bBoost)
						{
							if (b.nBoostID == 1)p.fSpeed -= b.fBonusValue;
							b.fBoostCounter = 0.0f;
							b.bRemove = true;
							p.bBoostActive = false;
						}
					}
				}
				p.listPlayerBoost.remove_if([&](const sBoost& b) {return b.bRemove; });

				//COUNTERS
				p.fGunReloadTimer += fElapsedTime * 0.5f;
				if (p.fGunReloadTimer >= p.fFireRate)
				{
					p.fGunReloadTimer -= p.fFireRate;
					p.bCanFire = true;
				}


				p.fSpecialCooldown += fElapsedTime;
				if (p.fSpecialCooldown >= 1.0f)
				{
					p.bSpeciallReady = true;
					p.fSpecialCooldown = 0.0f;
				}

				p.fMana += fElapsedTime * p.fManaRegeneration;
				if (p.fMana >= p.fMaxMana) p.fMana = p.fMaxMana;

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


			listFragments.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.bRemove; });
			listFragmentsPlayer1.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.bRemove; });
			listFragmentsPlayer2.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.bRemove; });



	//=============== Enemies ==================

				//Spawn 
			while (!listSpawns.empty() && dWorldPos >= listSpawns.front().dTriggerTime)
			{
				sEnemy e;
				e.def = listSpawns.front();
				if(e.def.nSpriteID == 2) SoundEngine->play2D("art/sounds/witchsLaugh.wav", GL_FALSE);
				e.pos = {
					(float)ScreenWidth() + sprEnemy[e.def.nSpriteID]->width,
					e.def.fOffset * (float)ScreenHeight() - sprEnemy[e.def.nSpriteID]->height
				};
				listSpawns.pop_front();
				if (listSpawns.empty()) bBoss = true;
				listEnemy.push_back(e);
			}

			// Update 
			for (auto& e : listEnemy)
			{
				olc::vf2d vNearestPlayer = { -10,200 };
				for (auto& p : listPlayers)
				{
					if ((p.pos - e.pos).mag() < (vNearestPlayer - e.pos).mag()) vNearestPlayer = p.pos;

				}
				e.Update(fElapsedTime, fWorldSpeed, listEnemyBullet, vNearestPlayer);
				if (e.def.nSpriteID == 3) fBossHealth = e.def.nEnemyLives;  //add bBoss to definition
				for (auto& p : listPlayers)
				{
					if (((e.pos + GetMiddle(sprEnemy[e.def.nSpriteID])) - (p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]))).mag() < (float)sprPlayer[p.nPlayerID - 1]->width / 1.95f && !p.bIsProtected)
					{
						if (p.fMana > p.fMaxMana) { p.fMaxMana -= p.fMaxMana / 2; p.nLives--; }
						SoundEngine->play2D("art/sounds/loose.wav", GL_FALSE);
						if (p.nLives <= 0)
						{
							Explode(p, p.pos, 1000);
							if (p.nPlayerID == 1) bPlayer1 = false;
							if (p.nPlayerID == 2) bPlayer2 = false;
							p.bDead = true;
							SoundEngine->play2D("art/sounds/explode.wav", GL_FALSE);
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
						b.bRemove = true;
						if (p.fMana >= p.fMaxMana/3) p.fMana -= p.fMaxMana / 2; 
						else p.nLives--; 
						SoundEngine->play2D("art/sounds/loose.wav", GL_FALSE);
						if (p.nLives <= 0)
						{
							Explode(p, p.pos, 1000);
							if (p.nPlayerID == 1) bPlayer1 = false;
							if (p.nPlayerID == 2) bPlayer2 = false;
							p.bDead = true;
							SoundEngine->play2D("art/sounds/explode.wav", GL_FALSE);
						}
						p.bIsProtected = true;
						p.fProtectionTimer = 0.0f;
					}
				}

			}

			//Remove Bullets
			listEnemyBullet.remove_if([&](const sBullet& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight() || b.bRemove; });
			//Remove
			listEnemy.remove_if([&](const sEnemy& e) {return (e.pos.x < 0.0f) || e.def.nEnemyLives <= 0; });
			listSplash.remove_if([&](const sSplash& s) {return s.fCounter >= 0.15f; });
			listDrops.remove_if([&](const sDrops& d) {return d.fCounter >= 5 || d.bRemove; });
		}
		if(bPause)
		{
			if (bResume)
			{
				if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bPause = false; bResume = false; }
				if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bResume = false;}
				if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bResume = false;}
			}
			else if (bRestart)
			{
				if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); restart(); bRestart = false; bPause = false; }
				if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bResume = true; bRestart = false; }
				if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfo = true; bRestart = false; }
			}
			else if (bInfo)
			{
				if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfoPanel = true; bInfo = false; }
				if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bInfo = false; }
				if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bInfo = false; }
			}
			else if (bQuit)
			{
				if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); exit(0); }
				if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfo = true; bQuit = false; }
				if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bResume = true; bQuit = false; }
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

				//SPLASH
				for (auto& s : listSplash)
				{
					DrawSprite(s.pos.x, s.pos.y, sprSplash[s.nID - 1]);
					s.pos.x += fElapsedTime * 50;
					s.fCounter += fElapsedTime;

				}
				//POWER UPS
				for (auto& d : listDrops)
				{
					if (d.fCounter < 3) 
					{
						DrawSprite(d.pos.x, d.pos.y, sprPowerUps[d.nID]);
					}
					else 
					{
						d.fFrameCounter += fElapsedTime * 2;
						if (d.fFrameCounter >= 1) d.fFrameCounter = 0.0f;
						if (d.fFrameCounter < 0.5f)
						{
							DrawSprite(d.pos.x, d.pos.y, sprPowerUps[d.nID]);
						}
					}
					if (d.fCounter > 5) d.bRemove = true;
					d.pos.x -= fWorldSpeed * fElapsedTime * 0.7f;
					d.fCounter += fElapsedTime;
				}

				//Player
				for (auto& p : listPlayers)
				{

					p.fAnimationFrame += fElapsedTime * 1.5;
					if (p.fAnimationFrame >= 1) p.fAnimationFrame = 0.0f;
					for (auto& b : p.listPlayerBoost) DrawSprite(p.pos.x + b.offset.x, p.pos.y + b.offset.y, sprBoost[p.nPlayerID - 1]);
					DrawSprite(p.pos.x, p.pos.y, ((p.bIsFire) ? sprPlayerAtack[p.nPlayerID - 1] : sprPlayer[p.nPlayerID - 1]));
					if (p.bIsProtected) 
					{
						if(p.fAnimationFrame < 0.5f)DrawSprite(p.pos.x - 10, p.pos.y - 8, sprShield[p.nPlayerID - 1]);
					}
					for (auto& b : p.listPlayerBullet) DrawSprite(b.pos.x, b.pos.y, sprPlayerBullet[b.nBulletTypeID + p.nPlayerID-1]);
					for (auto& b : p.listPlayerSpeciall) DrawSprite(b.pos.x, b.pos.y, (p.nPlayerID == 1) ? aSpecialP1.playAnimation(fElapsedTime) : aSpecialP2.playAnimation(fElapsedTime, 0.07f));
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
					ss<< std::setw(10) << std::setfill('0') << p.nScore;
					DrawString((float)ScreenWidth() * offset + 10, (float)ScreenHeight() * 0.035f, (ss.str() + "\n\nP" + std::to_string(p.nPlayerID) + ":"));
					for (int i = 0; i < (int)p.nLives; i++) DrawSprite((float)ScreenWidth() * offset + 40 + sprHealth[p.nPlayerID-1]->width*i, (float)ScreenHeight() * 0.07f, sprHealth[p.nPlayerID - 1]);
					//FillRect((float)ScreenWidth()* offset + 25, (float)ScreenHeight() * 0.12f + 3, (p.fMana / 100 * 60), 9, olc::CYAN);
					DrawPartialSprite((float)ScreenWidth()* offset + 10, (float)ScreenHeight() * 0.12f, sprManaFrame[p.nPlayerID-1],0,0,sprManaFrame[p.nPlayerID - 1]->width * (p.fMana / 100), sprManaFrame[p.nPlayerID - 1]->height);
				}

				SetPixelMode(olc::Pixel::NORMAL);

				//JOIN PLAYER 
				if (!bPlayer1 && !bStart && bGame)  DrawString((float)ScreenWidth() * 0.05f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
				if (!bPlayer2 && !bStart && bGame)  DrawString((float)ScreenWidth() * 0.8f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
	
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

				//BOSS BAR
				if (bBoss) 
				{
					DrawString((float)ScreenWidth() * 0.288f, (float)ScreenHeight() * 0.875f, "BOSS: ", olc::DARK_RED);
					FillRect((float)ScreenWidth() * 0.36f, (float)ScreenHeight() * 0.87f,(fBossHealth / 100 * 500), 10, olc::DARK_RED);
				}

				// ====WIN===
				if (listSpawns.empty() && listEnemy.empty() && bGame)
				{
					bBoss = false;
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
		return true;
	}

	bool OnUserDestroy()
	{
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