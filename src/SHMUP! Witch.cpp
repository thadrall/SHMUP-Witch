#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "patherns.h"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include <ctime>
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


	//Players
	float fP1pos;
	float fP2pos;

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
	bool bOptions = false;
	bool bOptionsMenu = false;
	bool bSounds = false;
	bool bSoundPanel = false;
	bool bMainMenu = false;
	bool bBack = false;
	bool bVolume = false;
	bool bMusic = false;
	bool bMusicON = true;
	bool bWin;
	bool bTimeOut = false;
	int nVolumeLvl = 5;

	//UI
	int nLastHiScore;
	int nHiScore = 0;
	float fTimer = 0;
	float fBlinkCounter;
	float fBossHealth;
	float fBoosMax;
	std::string txtHiScore = "0";
	int nBuffScore1 = 0;
	int nBuffScore2 = 0;
	int nMaxLevelTime = 55;

	//SCORE
	std::ofstream writeHiScore;
	std::ifstream readHiScore;

	//BOOSTS
	sBoost boostSpeed;
	sBoost boostFire;

	//ANIMATIONS
	sAnimation aSpecialP1;
	sAnimation aSpecialP2;

	//ENEMIES
	sEnemyDefinition sPumpkin;
	sEnemyDefinition sBat;
	sEnemyDefinition sGhost;
	sEnemyDefinition sMoon;
	sEnemyDefinition sWitch;

	//LEVEL
	bool bFirstSpawn = true;
	int nWave = 0;

	//FUNCTIONS

	void SpawnLevel(int nWave, int nPumpkins = 5, int nBats = 30, int nGhost = 2, int nWitch = 1, int nLevelDistance = 600) 
	{
			nLevelDistance += nWave * 50;
			nMaxLevelTime += 5;
			for (int i = 0; i < nPumpkins + nWave; i++)
			{
				sPumpkin.dTriggerTime = ((rand() % nLevelDistance/5) * 35 + i * 5)+1;
				sPumpkin.fOffset = (2 + (rand() % 7)) * 0.1;
				listSpawns.push_back(sPumpkin);
			}
			for (int i = 0; i < nBats + nWave * 5; i++)
			{
				sBat.dTriggerTime = (rand() % nLevelDistance / 50) * 400 + i+1;
				sBat.fOffset = (2 + (rand() % 7)) * 0.1;
				listSpawns.push_back(sBat);
			}
			for (int i = 0; i < nGhost + (int)(nWave / 2); i++)
			{
				sGhost.dTriggerTime = (rand() % nLevelDistance / 5) * 40+1;
				sGhost.fOffset = (2 + (rand() % 7)) * 0.1;
				listSpawns.push_back(sGhost);
			}
			for (int i = 0; i < nWitch + (int)(nWave/4); i++)
			{
				sWitch.dTriggerTime = (rand() % nLevelDistance / 2) * 16+1;
				sWitch.fOffset = (2 + (rand() % 7)) * 0.1;
				listSpawns.push_back(sWitch);
			}
			sMoon.dTriggerTime = nLevelDistance * 8;
			sMoon.fOffset = 0.5f;
			fBoosMax = 20 + nWave * 10;
			sMoon.nEnemyLives = 20 + nWave * 10;
			sMoon.bBoos = true;
			listSpawns.push_back(sMoon);
			listSpawns.sort([](const sEnemyDefinition& enemy1, const sEnemyDefinition& enemy2)
				{
					return enemy1.dTriggerTime < enemy2.dTriggerTime;
				});
			bFirstSpawn = false;
			listSpawnsBkp = listSpawns;

			

		
	}

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
	void Explode(olc::vf2d pos, int f = 10) {
		for (int i = 0; i < f; i++)
		{
			float fAngle = ((float)rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
			float fSpeed = ((float)rand() / (float)RAND_MAX) * 200.0f + 50;
			listFragments.push_back({pos,{fSpeed * cosf(fAngle), fSpeed * sinf(fAngle)}});
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
		listSpawns.clear();
		listDrops.clear();
		bGame = true;
		bStart = true;
		bPlayer1 = false;
		bPlayer2 = false;
		bBoss = false;
		nWave = 0;
		vSkyPos.x = 0;
		fCounter = 0.0f;
		fTimer = 0;
		SpawnLevel(nWave);
		SoundEngine->stopAllSounds();
		if (bMusicON)SoundEngine->play2D("art/sounds/slayer_south_of_heaven_8_bit.mp3", GL_TRUE);
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
			int nRand = 1 + (rand() % 100);
			if (nRand <= 70) d.nID = 1;
			if (95 > nRand > 50) d.nID = 2;
			if (nRand >= 95) d.nID = 2;
			listDrops.push_back(d);
		}
	}
	void loseLife(Player& p)
	{
		if (p.fMana >= p.fMaxMana / 2)p.fMana -= p.fMaxMana / 2;
		else p.nLives--;
		SoundEngine->play2D("art/sounds/loose.wav", GL_FALSE);
		if (p.nLives <= 0)
		{
			Explode(p, p.pos, 1000);
			if (p.nPlayerID == 1) { bPlayer1 = false; nBuffScore1 = p.nScore; }
			if (p.nPlayerID == 2) { bPlayer2 = false; nBuffScore2 = p.nScore; }
			p.bDead = true;
			SoundEngine->play2D("art/sounds/explode.wav", GL_FALSE);
		}
		p.bIsProtected = true;
		p.fProtectionTimer = 0.0f;
	}


	//==============================================================CREATE=========================================================

	bool OnUserCreate() override
	{
		srand(std::time(0));
		bGame = true;
		bStart = true;
		bPause = false;
		vScreenSize = { (float)ScreenWidth(), (float)ScreenHeight() };

		//BOOSTS
		boostSpeed.nBoostID = 1;
		boostSpeed.offset = { -23,19 };
		boostSpeed.fDuration = 8.0f;
		boostSpeed.fBonusValue = 120.0f;
		boostSpeed.fBoostCounter = 0.0f;
		boostSpeed.fManaCost = 10.0f;

		boostFire.nBoostID = 2;
		boostFire.offset = { 0,0 };
		boostFire.fDuration = 8.0f;
		boostFire.fBonusValue = 0.5f;
		boostFire.fBoostCounter = 0.0f;
		boostFire.fManaCost = 20.0f;


		//LOAD HI-SCORE
		readHiScore.open("data/hs.txt", std::ios_base::in);
		if (readHiScore.is_open()) {
			while (readHiScore.good()) {
				std::getline(readHiScore, txtHiScore);
			}
		}
		nHiScore = std::stoi(txtHiScore);

		//SOUND
		SoundEngine = irrklang::createIrrKlangDevice();
		SoundEngine->setSoundVolume(nVolumeLvl * 0.1f);
		if (bMusicON)SoundEngine->play2D("art/sounds/slayer_south_of_heaven_8_bit.mp3", GL_TRUE);

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

		sprPlayerBullet[0] = new olc::Sprite("art/player1bullet2.png");
		sprPlayerBullet[1] = new olc::Sprite("art/player2bullet2.png");
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
		sprPowerUps[1] = new olc::Sprite("art/potion2.png");
		sprPowerUps[2] = new olc::Sprite("art/cauldron.png");

		sprManaFrame[0] = new olc::Sprite("art/manaP1d.png");
		sprManaFrame[1] = new olc::Sprite("art/manaP2d.png");

		sprBoost[0] = new olc::Sprite("art/boost1.png");
		sprBoost[1] = new olc::Sprite("art/boost2.png");
		sprBoost[2] = new olc::Sprite("art/boost3a.png");
		sprBoost[3] = new olc::Sprite("art/boost4.png");

		sprUISpecialA[0] = new olc::Sprite("art/UI/special0.png");
		sprUISpecialA[1] = new olc::Sprite("art/UI/special1.png");
		sprUISpecialA[2] = new olc::Sprite("art/UI/special2.png");
		sprUISpecialA[3] = new olc::Sprite("art/UI/special3.png");
		sprUISpecialA[4] = new olc::Sprite("art/UI/special4.png");
		sprUISpecialA[5] = new olc::Sprite("art/UI/special5.png");

		sprUISpecialD[0] = new olc::Sprite("art/UI/special0b.png");
		sprUISpecialD[1] = new olc::Sprite("art/UI/special1b.png");
		sprUISpecialD[2] = new olc::Sprite("art/UI/special2b.png");
		sprUISpecialD[3] = new olc::Sprite("art/UI/special3b.png");
		sprUISpecialD[4] = new olc::Sprite("art/UI/special4b.png");
		sprUISpecialD[5] = new olc::Sprite("art/UI/special5b.png");
		
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell0.png"));
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell1.png"));
		aSpecialP1.listFrame.push_back(new olc::Sprite("art/animation/spell2.png"));

		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning0.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning1.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning2.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning3.png"));
		aSpecialP2.listFrame.push_back(new olc::Sprite("art/animation/lightning4.png"));

		sprSpeedBoost[0] = new olc::Sprite("art/UI/speedBoost.png");
		sprSpeedBoost[1] = new olc::Sprite("art/UI/speedBoostb.png");

		sprBossBar[0] = new olc::Sprite("art/UI/bossBar.png");
		sprBossBar[1] = new olc::Sprite("art/UI/bossBarFr.png");
		
		// ==========================SCENE=============================

		//SKY
		for (int i = 0; i < arySky.size(); i++)
		{
			auto& s = arySky[i];
			s.x = sprSky[i%2]->width * i;
			s.y =  0;
		}

		//MOUNTAINS
		for (int i = 0; i < aryMountains.size(); i++)
		{
			auto& m = aryMountains[i];
			m.x = sprMountains[i]->width * i;
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


		//			ENEMY  MODELS
		
		//Pumpkin
		sPumpkin.nSpriteID = 0;
		sPumpkin.nEnemyLives = 2;
		sPumpkin.funcMove = Move_None;
		sPumpkin.funcFire = Fire_Triple;
		sPumpkin.nDropRate = 10;
		sPumpkin.nScore = 200;
		//Bat
		sBat.nSpriteID = 4;
		sBat.nEnemyLives = 1;
		sBat.funcMove = Move_Bat;
		sBat.funcFire = Fire_None;
		sBat.nDropRate = 5;
		sBat.nScore = 50;
		//Ghost
		sGhost.nSpriteID = 1;
		sGhost.nEnemyLives = 4;
		sGhost.funcMove = Move_Sin;
		sGhost.funcFire = Fire_Straight;
		sGhost.nDropRate = 20;
		sGhost.nScore = 300;
		//Witch
		sWitch.nSpriteID = 2;
		sWitch.nEnemyLives = 2;
		sWitch.funcMove = Move_Sin_Fast;
		sWitch.funcFire = Fire_Fast;
		sWitch.nDropRate = 50;
		sWitch.nScore = 500;
		//Moon
		sMoon.nSpriteID = 3;
		sMoon.nEnemyLives = 50;
		sMoon.funcMove = Move_Moon;
		sMoon.funcFire = Fire_Moon;
		sMoon.nDropRate = 100;
		sMoon.bBoos = true;
		sMoon.nScore = 1500;

	
		return true;
	}

	//============================================================== UPDATE ==============================================================

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (listSpawns.empty() && bFirstSpawn) {
			SpawnLevel(nWave);
		}

		if (GetKey(olc::ESCAPE).bPressed && bGame)
			if (!bPause)
			{
				bPause = true;
				bMainMenu = true;
				//bResume = true;
				SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE);
			}

		if (!bPause) {
			// ==================== Player ================
			if(bGame && !bStart && !bWin)fTimer += fElapsedTime;
			if (GetKey(olc::SPACE).bPressed && !bPlayer1 && bGame)
			{
				Player* player = new Player(1);
				listPlayers.push_back(*player);
				bPlayer1 = true;
				if (bStart) bStart = false;
			}

			if (GetMouse(0).bPressed && !bPlayer2 && bGame)
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
					//MOVEMENT
					if (GetKey(olc::W).bHeld) {p.vInput.y -= p.fInputSens * fElapsedTime; p.bInput = true;}
					if (!GetKey(olc::W).bHeld && p.vInput.y < p.pos.y )p.vInput.y += p.fInputSens * fElapsedTime;
					if (GetKey(olc::S).bHeld){ p.vInput.y += p.fInputSens * fElapsedTime; p.bInput = true;}
					if (!GetKey(olc::S).bHeld && p.vInput.y > p.pos.y )p.vInput.y -= p.fInputSens* fElapsedTime;
					if (GetKey(olc::A).bHeld){ p.vInput.x -= p.fInputSens * fElapsedTime; p.bInput = true;}
					if (!GetKey(olc::A).bHeld && p.vInput.x < p.pos.x )p.vInput.x += p.fInputSens * fElapsedTime;
					if (GetKey(olc::D).bHeld){ p.vInput.x += p.fInputSens * fElapsedTime; p.bInput = true;}
					if (!GetKey(olc::D).bHeld && p.vInput.x > p.pos.x )p.vInput.x -= p.fInputSens * fElapsedTime;

					if (GetKey(olc::D).bHeld && GetKey(olc::A).bHeld)p.vInput.x -= p.fInputSens/2 * fElapsedTime;;
					//DEADZONE
					if ((!GetKey(olc::D).bHeld) && (!GetKey(olc::A).bHeld)) {
						if (std::abs(p.vInput.x - p.pos.x) <= 0.1f)p.vInput.x = p.pos.x;

					}
					if ((!GetKey(olc::W).bHeld) && (!GetKey(olc::S).bHeld)) {
						
						if (std::abs(p.vInput.y - p.pos.y) <= 0.5f)p.vInput.y = p.pos.y;
					}

					//FIRE + SPELLS
					p.bIsFire = (GetKey(olc::SPACE).bHeld) ? 1 : 0;
					p.bActiveSpecial = (GetKey(olc::ALT).bHeld) ? 1 : 0;
					if (GetKey(olc::K1).bPressed) { p.bActiveSpecial = false; p.nActiveSpecialID = 1;}
					if (GetKey(olc::K2).bPressed) { p.bActiveSpecial = false; p.nActiveSpecialID = 2;}
					if (GetKey(olc::K3).bPressed) { p.bActiveSpecial = false; p.nActiveSpecialID = 3;}
				}
				//Player2
				if (p.nPlayerID == 2)
				{
					//MOVEMENT
					if (GetKey(olc::UP).bHeld) { p.vInput.y -= p.fInputSens * fElapsedTime; p.bInput = true; }
					if (!GetKey(olc::UP).bHeld && p.vInput.y < p.pos.y)p.vInput.y += p.fInputSens * fElapsedTime;
					if (GetKey(olc::DOWN).bHeld) { p.vInput.y += p.fInputSens * fElapsedTime; p.bInput = true; }
					if (!GetKey(olc::DOWN).bHeld && p.vInput.y > p.pos.y)p.vInput.y -= p.fInputSens * fElapsedTime;
					if (GetKey(olc::LEFT).bHeld) { p.vInput.x -= p.fInputSens * fElapsedTime; p.bInput = true; }
					if (!GetKey(olc::LEFT).bHeld && p.vInput.x < p.pos.x)p.vInput.x += p.fInputSens * fElapsedTime;
					if (GetKey(olc::RIGHT).bHeld) { p.vInput.x += p.fInputSens * fElapsedTime; p.bInput = true; }
					if (!GetKey(olc::RIGHT).bHeld && p.vInput.x > p.pos.x)p.vInput.x -= p.fInputSens * fElapsedTime;

					if (GetKey(olc::RIGHT).bHeld && GetKey(olc::LEFT).bHeld) p.vInput.x = 0;
					//DEADZONE
					if ((!GetKey(olc::RIGHT).bHeld) && (!GetKey(olc::LEFT).bHeld)) {
						if (std::abs(p.vInput.x - p.pos.x) <= 0.1f)p.vInput.x = p.pos.x;

					}
					if ((!GetKey(olc::UP).bHeld) && (!GetKey(olc::DOWN).bHeld)) {

						if (std::abs(p.vInput.y - p.pos.y) <= 0.5f)p.vInput.y = p.pos.y;
					}


					//FIRE + SPELLS
					p.bIsFire = (GetMouse(0).bHeld) ? 1 : 0;
					p.bActiveSpecial = (GetMouse(1).bHeld) ? 1 : 0;
					if (GetKey(olc::DEL).bPressed) {p.bActiveSpecial = false;  p.nActiveSpecialID = 1;}
					if (GetKey(olc::END).bPressed) {p.bActiveSpecial = false;  p.nActiveSpecialID = 2;}		
					if (GetKey(olc::PGDN).bPressed) {p.bActiveSpecial = false;  p.nActiveSpecialID = 3;}
				}

				
								//INPUT CLAMP
				if (p.vInput.x >= p.pos.x + 1)p.vInput.x = p.pos.x + 1;
				if (p.vInput.x <= p.pos.x - 1)p.vInput.x = p.pos.x - 1;
				if (p.vInput.y >= p.pos.y + 1)p.vInput.y = p.pos.y + 1;
				if (p.vInput.y <= p.pos.y - 1)p.vInput.y = p.pos.y - 1;

				if (p.vInput.x <= 0) p.vInput.x = 0;
				if (p.vInput.y <= 0) p.vInput.y = 0;
				if (p.vInput.x + (float)sprPlayer[p.nPlayerID - 1]->width >= (float)ScreenWidth()) p.vInput.x = (float)ScreenWidth() - (float)sprPlayer[p.nPlayerID - 1]->width;
				if (p.vInput.y + (float)sprPlayer[p.nPlayerID - 1]->height >= (float)ScreenHeight()) p.vInput.y = (float)ScreenHeight() - (float)sprPlayer[p.nPlayerID - 1]->height;


				//MOVEMENT
				p.vel = (p.vInput - p.pos) * p.fSpeed;
				if(p.pos.x > 20)p.vel.x -=  p.pos.x/2;

				p.vInput += p.vel * fElapsedTime;
				p.pos += p.vel * fElapsedTime;
				
				//Update global pos
				if (p.nPlayerID == 1)fP1pos = p.pos.x;
				else fP2pos = p.pos.x;

				//CLAMP
				if (p.pos.x <= 0) p.pos.x = 0;
				if (p.pos.y <= 0) p.pos.y = 0;
				if (p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width >= (float)ScreenWidth()) p.pos.x = (float)ScreenWidth() - (float)sprPlayer[p.nPlayerID - 1]->width;
				if (p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height >= (float)ScreenHeight()) p.pos.y = (float)ScreenHeight() - (float)sprPlayer[p.nPlayerID - 1]->height ;


				//POWER UPS
				for (auto& d : listDrops)
				{
					if (((p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1])) - (d.pos + GetMiddle(sprPowerUps[d.nID]))).mag() <= sprPowerUps[d.nID]->width) 
					{
						SoundEngine->play2D("art/sounds/powerup.wav", GL_FALSE);
						d.bRemove = true;
						if (d.nID == 0) p.fManaRegeneration += 0.2;
						if (d.nID == 1) p.fMana += 20.0f;
						if (d.nID == 2) (p.nLives < 4) ? p.nLives++ : p.nScore += 5000;
					}
				}

				//FIRE
				if (p.bIsFire && p.bCanFire)
				{
					sBullet b;
					b.pos = { p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width, p.pos.y + (float)sprPlayer[p.nPlayerID - 1]->height / 3 };
					b.vel = p.vel + olc::vf2d {p.fBulletSpeed, -p.vel.y * 0.3f};
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
					if (p.bActiveSpecial && p.nActiveSpecialID == 1 && p.fMana > p.fSpecialManaCost && p.bSpecialReady)
					{
						sSpell s;
						s.pos = { p.pos.x + (float)sprPlayer[p.nPlayerID - 1]->width, p.pos.y - 15 };
						if (s.pos.y <= 1) s.pos.y = 1;
						s.vel = { s.fSpellSpeed, 0.0f };
						p.fMana -= p.fSpecialManaCost;
						p.bSpecialReady = false;
						p.listPlayerSpell.push_back(s);
						if (p.nPlayerID == 1) SoundEngine->play2D("art/sounds/warp.wav", GL_FALSE);
						else SoundEngine->play2D("art/sounds/electricShock.wav", GL_FALSE);
					}

					//SPELL
					for (auto& s : p.listPlayerSpell)
					{
						s.pos += (s.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
						s.fCounter += fElapsedTime;
						if (s.fCounter > s.fSpellCooldown) {
							s.fCounter = 0.0f;
							s.bReady = true;
						}
						for (auto& e : listEnemy)
						{
							if (((s.pos + GetMiddle(aSpecialP1.playAnimation(fElapsedTime))) - (e.pos + GetMiddle(sprEnemy[e.def.nSpriteID]))).mag() < ((float)sprEnemy[e.def.nSpriteID]->width + (float)aSpecialP1.playAnimation(fElapsedTime)->width) / 2.0f && s.bReady)
							{
								e.def.nEnemyLives--;
								Explode(p, s.pos, 8);
								sSplash sp;
								sp.pos = e.pos;
								sp.nID = p.nPlayerID;
								sp.fCounter = 0.0f;
								listSplash.push_back(sp);
								if (p.nPlayerID == 1)SoundEngine->play2D("art/sounds/zap.wav", GL_FALSE);
								else SoundEngine->play2D("art/sounds/zap.mp3", GL_FALSE);
								if (e.def.nEnemyLives <= 0)
								{
									kill(e, p);
								}
								s.bReady = false;
							}
						}
					}
					p.listPlayerSpell.remove_if([&](const sSpell& b) {return b.pos.x < 0.0f || b.pos.y < 0.0f || b.pos.x >(float)ScreenWidth() || b.pos.y >(float)ScreenHeight(); });


					//BOOST
					if (p.bActiveSpecial && p.nActiveSpecialID == 2 && !p.bBoostActive)
					{
						if (p.fMana >= boostSpeed.fManaCost / 3) {
							p.listPlayerBoost.push_back(boostSpeed);
							p.fSpeed += boostSpeed.fBonusValue;
							p.fBulletSpeed += boostSpeed.fBonusValue;
							p.bBoostActive = true;
						}
					}
					if (p.bActiveSpecial && p.nActiveSpecialID == 3 && !p.bBoostActive)
					{
						if (p.fMana >= boostFire.fManaCost / 3) {
							p.listPlayerBoost.push_back(boostFire);
							p.fBoostBuffor = (p.fFireRate * boostFire.fBonusValue);
							p.fFireRate -= p.fBoostBuffor;
							p.bBoostActive = true;
						}
					}

					if (p.bBoostActive)
					{
						for (auto& b : p.listPlayerBoost)
						{
							p.fMana -= b.fManaCost * fElapsedTime;
							b.fCharge += fElapsedTime;
							if (b.fCharge > 0.15f) {
								b.fCharge = 0.0f;
								b.bReady = true;
							}
							
							if(b.nBoostID == 1){
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
							}

							b.fBoostCounter += fElapsedTime;
							if (b.fBoostCounter >= b.fDuration || p.fMana <= 1 || !p.bActiveSpecial)
							{
								if (b.nBoostID == 1) {
									p.fSpeed -= b.fBonusValue;
									p.fBulletSpeed -= b.fBonusValue;
								}
								if (b.nBoostID == 2)p.fFireRate += p.fBoostBuffor;
								b.fBoostCounter = 0.0f;
								b.bRemove = true;
								p.bBoostActive = false;
							}
						}
					}
					p.listPlayerBoost.remove_if([&](const sBoost& b) {return b.bRemove; });
				
				//COUNTERS
				p.fGunReloadTimer += fElapsedTime;
				if (p.fGunReloadTimer >= p.fFireRate)
				{
					p.fGunReloadTimer -= p.fFireRate;
					p.bCanFire = true;
				}

				p.fSpecialTimer += fElapsedTime;
				if (p.fSpecialTimer >= p.fSpeciallCooldown)
				{
					p.fSpecialTimer = 0.0f;
					p.bSpecialReady = true;
				}

				p.fMana += fElapsedTime * p.fManaRegeneration;
				if (p.fMana >= p.fMaxMana) p.fMana = p.fMaxMana;

				p.fProtectionTimer += fElapsedTime;
				if (p.fProtectionTimer >= p.fShieldDuration)
				{
					p.bIsProtected = false;
				}
			}
			//REMOVE
			listPlayers.remove_if([&](const Player& p) {return p.bDead || bTimeOut; });


	//============= World =========

			if (!bStart) dWorldPos += fWorldSpeed * fElapsedTime;
			//World Speed
			if (bPlayer1 && bPlayer2) {
				float fMiddlePoint = (fP1pos > fP2pos) ? fP1pos - (fP1pos - fP2pos)/2 : fP2pos - (fP2pos - fP1pos)/2;
				fWorldSpeed = (fMiddlePoint > (float)ScreenWidth() * 0.5f) ? fMiddlePoint / 2 + 70 : fMiddlePoint / 3 + 100;
			}
			else if (bPlayer1)fWorldSpeed = (fP1pos > (float)ScreenWidth() * 0.5f) ? fP1pos / 2 + 70 : fP1pos / 3 + 100;
			else if(bPlayer2)fWorldSpeed = (fP2pos > (float)ScreenWidth() * 0.5f) ? fP2pos / 2 + 70 : fP2pos / 3 + 100;

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
				if (e.def.bBoos) fBossHealth = e.def.nEnemyLives; 
				for (auto& p : listPlayers)
				{
					if (((e.pos + GetMiddle(sprEnemy[e.def.nSpriteID])) - (p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]))).mag() < (float)sprPlayer[p.nPlayerID - 1]->width / 1.95f && !p.bIsProtected)
					{
						loseLife(p);
					}
				}
			};

			// Fire
			for (auto& b : listEnemyBullet)
			{
				b.pos += (b.vel - olc::vf2d(fWorldSpeed, 0.0f)) * fElapsedTime;
				for (auto& p : listPlayers)
				{
					if (((b.pos + GetMiddle(sprEnemyBullet[b.nBulletTypeID])) - (p.pos + GetMiddle(sprPlayer[p.nPlayerID - 1]))).mag() < (float)sprPlayer[p.nPlayerID - 1]->width / 2.2f && !p.bIsProtected)
					{
						b.bRemove = true;
						loseLife(p);
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

		//=========================================================== PAUSE MENU ===============================================================
		if (bPause)
		{
			//	SAVE HI SCORE
			readHiScore.open("data/hs.txt", std::ios_base::in);
			if (readHiScore.is_open()) {
				while (readHiScore.good()) {
					std::getline(readHiScore, txtHiScore);
				}
			}
			nLastHiScore = std::stoi(txtHiScore);
			if (nHiScore > nLastHiScore)
			{
				writeHiScore.open("data/hs.txt", std::ios_base::trunc);
				if (writeHiScore.is_open()) {
					writeHiScore << std::to_string(nHiScore);
					writeHiScore.close();
				}
			}

			//							MAIN MENU
			if(bMainMenu)
			{
				if (bResume)
				{
					if (GetKey(olc::ENTER).bPressed || (GetKey(olc::ESCAPE).bPressed)) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bPause = false; bResume = false; bMainMenu = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bResume = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bResume = false; }
				}
				else if (bRestart)
				{
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); restart(); bRestart = false; bPause = false; bMainMenu = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bResume = true; bRestart = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptions = true; bRestart = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bPause = false; bRestart = false; bMainMenu = false; }
				}
				else if (bOptions)
				{
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = true; bInfo = true; bMainMenu = false; bOptions = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bOptions = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bOptions = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bPause = false; bOptions = false; bMainMenu = false; }
				}

				else if (bQuit)
				{
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); exit(0); }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptions = true; bQuit = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bResume = true; bQuit = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bPause = false; bQuit = false; bMainMenu = false; }
				}
				else bResume = true;
			}
			//								 OPTIONS
			else if (bOptionsMenu)
			{
				if (bInfo)
				{
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfoPanel = true; bBack = true; bInfo = false; bOptionsMenu = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bBack = true; bInfo = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bSounds = true; bInfo = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = false; bMainMenu = true; bResume = true; bInfo = false; }
				}
				else if (bSounds)
				{
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bSoundPanel = true; bVolume = true; bSounds = false; bOptionsMenu = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfo = true; bSounds = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bBack = true; bSounds = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = false; bMainMenu = true; bResume = true; bSounds = false; }
				}
				else if (bBack)
				{
					if (GetKey(olc::ENTER).bPressed || GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = false; bMainMenu = true; bResume = true; bBack = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bSounds = true; bBack = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bInfo = true; bBack = false; }
				}
			}
			//						SOUND OPTIONS
			else if (bSoundPanel)
			{
				if (bVolume)
				{
					if (GetKey(olc::LEFT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); nVolumeLvl--; SoundEngine->setSoundVolume(nVolumeLvl * 0.1f); }
					if (GetKey(olc::RIGHT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); nVolumeLvl++; SoundEngine->setSoundVolume(nVolumeLvl * 0.1f); }
					
					if (nVolumeLvl <= 0) nVolumeLvl = 0;
					if (nVolumeLvl >= 10) nVolumeLvl = 10;

					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bBack = true; bVolume = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bMusic = true; bVolume = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = true; bInfo = true; bSoundPanel = false; bVolume = false; }
				}
				else if (bMusic)
				{
					if (GetKey(olc::ENTER).bPressed) {
						if (bMusicON) { SoundEngine->stopAllSounds(); bMusicON = false; }
						else { SoundEngine->play2D("art/sounds/slayer_south_of_heaven_8_bit.mp3", GL_TRUE); bMusicON = true; }
					}
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bVolume = true; bMusic = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bBack = true; bMusic = false; }
					if (GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = true; bInfo = true; bSoundPanel = false; bMusic = false; }
				}
				else if (bBack)
				{
					if (GetKey(olc::ENTER).bPressed || GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = true; bInfo = true; bSoundPanel = false; bBack = false; }
					if (GetKey(olc::UP).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bMusic = true; bBack = false; }
					if (GetKey(olc::DOWN).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bVolume = true; bBack = false; }
				}
			}

			else if (bInfoPanel)
			{
				if (bBack)
				{
					if (GetKey(olc::ENTER).bPressed || GetKey(olc::ESCAPE).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bOptionsMenu = true; bBack = false; bInfo = true; bInfoPanel = false; }
				}
			}

			else if(bWin)
			{
				 if (bRestart)
				 {
					if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); restart(); bRestart = false; bPause = false; bWin = false; }
					if (GetKey(olc::RIGHT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bRestart = false; }
					if (GetKey(olc::LEFT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bQuit = true; bRestart = false; }
				 }

				 else if (bQuit)
				 {
					 if (GetKey(olc::ENTER).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); exit(0); }
					 if (GetKey(olc::RIGHT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bQuit = false; }
					 if (GetKey(olc::LEFT).bPressed) { SoundEngine->play2D("art/sounds/ping.wav", GL_FALSE); bRestart = true; bQuit = false; }
				 }

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

					s.x -= fWorldSpeed * 0.001f;
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
					m.y = (float)ScreenHeight() * 0.1f;
					m.x -= fWorldSpeed * 0.013f;
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
					if(!bPause){d.pos.x -= fWorldSpeed * fElapsedTime * 0.7f;
					d.fCounter += fElapsedTime; }
				}

				//Player
				for (auto& p : listPlayers)
				{
					//Frame counter
					p.fAnimationFrame += fElapsedTime * 1.5;
					if (p.fAnimationFrame >= 1) p.fAnimationFrame = 0.0f;
					//Boost
					for (auto& b : p.listPlayerBoost) DrawSprite(p.pos.x + b.offset.x, p.pos.y + b.offset.y, sprBoost[((b.nBoostID-1) * 2) + (p.nPlayerID - 1)]);
					DrawSprite(p.pos.x, p.pos.y, ((p.bIsFire) ? sprPlayerAtack[p.nPlayerID - 1] : sprPlayer[p.nPlayerID - 1]));
					//Shield
					if (p.bIsProtected) 
					{
						if(p.fAnimationFrame < 0.5f)DrawSprite(p.pos.x - 10, p.pos.y - 8, sprShield[p.nPlayerID - 1]);
					}
					//Bulet
					for (auto& b : p.listPlayerBullet) DrawSprite(b.pos.x, b.pos.y, sprPlayerBullet[b.nBulletTypeID + p.nPlayerID-1]);
					//Spell
					for (auto& b : p.listPlayerSpell) DrawSprite(b.pos.x, b.pos.y, (p.nPlayerID == 1) ? aSpecialP1.playAnimation(fElapsedTime) : aSpecialP2.playAnimation(fElapsedTime, 0.07f));
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
					//offset
					float offset = (p.nPlayerID - 1) ? 0.81f : 0.05f;
					//Score
					std::ostringstream ss;
					ss<< std::setw(10) << std::setfill('0') << p.nScore;
					DrawString((float)ScreenWidth() * offset + 10, (float)ScreenHeight() * 0.035f, (ss.str() + "\n\nP" + std::to_string(p.nPlayerID) + ":"));
					//Lives
					for (int i = 0; i < (int)p.nLives; i++) DrawSprite((float)ScreenWidth() * offset + 40 + sprHealth[p.nPlayerID-1]->width*i, (float)ScreenHeight() * 0.07f, sprHealth[p.nPlayerID - 1]);
					//Mana
					DrawPartialSprite((float)ScreenWidth()* offset + 10, (float)ScreenHeight() * 0.12f, sprManaFrame[p.nPlayerID-1],0,0,sprManaFrame[p.nPlayerID - 1]->width * (p.fMana / p.fMaxMana), sprManaFrame[p.nPlayerID - 1]->height);
					//Special
					DrawString((float)ScreenWidth()* offset -2 , (float)ScreenHeight() * 0.85f, ((p.nPlayerID == 1) ? " 1" : "DEL"), ((p.nActiveSpecialID == 1) ? ((p.nPlayerID == 1) ? olc::DARK_GREEN : olc::DARK_CYAN) : olc::VERY_DARK_GREY));
					DrawString((float)ScreenWidth()* offset + 33, (float)ScreenHeight() * 0.85f, ((p.nPlayerID == 1) ? "  2" : "END"), ((p.nActiveSpecialID == 2) ? ((p.nPlayerID == 1) ? olc::DARK_GREEN : olc::DARK_CYAN) : olc::VERY_DARK_GREY));
					DrawString((float)ScreenWidth()* offset + 72, (float)ScreenHeight() * 0.85f, ((p.nPlayerID == 1) ? "  3" : "PGDN"), ((p.nActiveSpecialID == 3) ? ((p.nPlayerID == 1) ? olc::DARK_GREEN : olc::DARK_CYAN) : olc::VERY_DARK_GREY));
					DrawString((float)ScreenWidth()* offset + 12, (float)ScreenHeight() * 0.96f, ((p.nPlayerID == 1) ? "ALT = CAST" : "RMB = CAST"), ((p.bActiveSpecial) ? ((p.nPlayerID == 1) ? olc::DARK_GREEN : olc::DARK_CYAN) : olc::VERY_DARK_GREY));
					DrawSprite((float)ScreenWidth()* offset - 8 , (float)ScreenHeight() * 0.87f, ((p.nActiveSpecialID == 1) ? sprUISpecialA[p.nPlayerID - 1] : sprUISpecialD[p.nPlayerID - 1]));
					DrawSprite((float)ScreenWidth()* offset + 30, (float)ScreenHeight() * 0.87f, ((p.nActiveSpecialID == 2) ? sprUISpecialA[p.nPlayerID + 1] : sprUISpecialD[p.nPlayerID + 1]));
					DrawSprite((float)ScreenWidth()* offset + 70, (float)ScreenHeight() * 0.87f, ((p.nActiveSpecialID == 3) ? sprUISpecialA[p.nPlayerID + 3] : sprUISpecialD[p.nPlayerID + 3]));
				}
				//BOSS BAR
				if (bBoss)
				{
					DrawString((float)ScreenWidth() * 0.23f, (float)ScreenHeight() * 0.875f, "BOSS: ", olc::DARK_MAGENTA);
					DrawPartialSprite((float)ScreenWidth() * 0.3f, (float)ScreenHeight() * 0.86f, sprBossBar[0], 0, 0, (fBossHealth / fBoosMax * 250), 20);
					DrawSprite((float)ScreenWidth() * 0.3f, (float)ScreenHeight() * 0.86f, sprBossBar[1]);
				}

				SetPixelMode(olc::Pixel::NORMAL);

				//JOIN PLAYER 
				if (!bPlayer1 && !bStart) {
					DrawString((float)ScreenWidth() * 0.02f, (float)ScreenHeight() * 0.02f, "SCORE:" + std::to_string(nBuffScore1), olc::DARK_GREY, 1U);
					if(bGame)DrawString((float)ScreenWidth() * 0.02f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
				}
				if (!bPlayer2 && !bStart){
					DrawString((float)ScreenWidth() * 0.82f, (float)ScreenHeight() * 0.02f, "SCORE:" + std::to_string(nBuffScore2), olc::DARK_GREY, 1U);
					if (bGame)DrawString((float)ScreenWidth() * 0.82f, (float)ScreenHeight() * 0.055f, "PRESS FIRE\n TO START!", olc::Pixel(fMagic * 20.0f + 60, fMagic * 20.0f + 60, fMagic * 20.0f + 60));
				}
	
				//START
				if (bStart && !bPause)
				{
					DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.25f, "SHMUP!", olc::WHITE, 3U);
					DrawString((float)ScreenWidth() * 0.36f, (float)ScreenHeight() * 0.38f, "WITCH", olc::WHITE, 5U);
					DrawString((float)ScreenWidth() * 0.39f, (float)ScreenHeight() * 0.7f, "PRESS FIRE TO START...", olc::Pixel(fMagic * 40.0f + 80, fMagic * 40.0f + 80, fMagic * 40.0f + 80));
				}
				//HI SCORE
				auto sHiScore = std::to_string(nHiScore);
				DrawString((float)ScreenWidth() * 0.41f, (float)ScreenHeight() * 0.95f, "HI SCORE: " + std::to_string(nHiScore), olc::DARK_GREY);

				//Wave

				if(!bStart)DrawString((float)ScreenWidth() * 0.47f, (float)ScreenHeight() * 0.02f, "WAVE:" + std::to_string(nWave+1), olc::DARK_GREY);
			
				//TIMER
				if(bGame && !bStart)
				{
					DrawString((float)ScreenWidth() * 0.47f, (float)ScreenHeight() * 0.05f, "TIME:", olc::DARK_GREY);
					DrawString((float)ScreenWidth() * 0.48f, (float)ScreenHeight() * 0.082f, std::to_string((int)(nMaxLevelTime - fTimer)), ((nMaxLevelTime - fTimer)<20) ? magicPixel : olc::DARK_GREY, 2U);
				}

			

				// ====WIN===
				if (listSpawns.empty() && listEnemy.empty() && bGame)
				{	
					bWin = true;
					bBoss = false;
					if(!bPause)fCounter += fElapsedTime;
					if (!bPause)DrawString((float)ScreenWidth() * 0.28f, (float)ScreenHeight() * 0.22f, "TIME BONUS: " + std::to_string((int)(nMaxLevelTime - fTimer) * 50), olc::GREY, 2U);

					if (fCounter > 1.0f)
					{
						if(!bPause)DrawString((float)ScreenWidth() * 0.4225f, (float)ScreenHeight() * 0.32f, "WAVE: " + std::to_string(nWave+2) , olc::WHITE, 2U);
						if (!bPause)DrawString((float)ScreenWidth() * 0.43f, (float)ScreenHeight() * 0.5f, "GO!>>", magicPixel, 3U);
						DrawString((float)ScreenWidth() * 0.48f, (float)ScreenHeight() * 0.66f, std::to_string( 4 - (int)fCounter), magicPixel, 3U);
						if (fCounter > 5) 
							{
							nWave++;
								dWorldPos = 0; 
								SpawnLevel(nWave);
								fCounter = 0.0f;
								for (auto& p : listPlayers) {
									p.nScore += (int)(nMaxLevelTime - fTimer) * 50;
								}
								fTimer = 0;
								bWin = false;
							}
						
					}
				}
				//=====TIMEOUT=====

				bTimeOut = (fTimer > nMaxLevelTime);

				//  ===LOSE===
				if ((listPlayers.empty() && !bStart) || bTimeOut)
				{
					bWin = false;
					bBoss = false;
					bPlayer1 = false;
					bPlayer2 = false;
					bGame = false;
					fWorldSpeed = 30.0f;
					listEnemy.clear();
					listSpawns.clear();
					fCounter += fElapsedTime;
					if (fCounter > 1.5f) {
						if (GetKey(olc::ESCAPE).bPressed)restart();
						if (GetKey(olc::ENTER).bPressed) fCounter += 1;
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.32f, (bTimeOut) ? "TIME OUT" : "GAME OVER", olc::WHITE, 2U);
						DrawString((float)ScreenWidth() * 0.46f, (float)ScreenHeight() * 0.42f, "CONTINUE?", olc::WHITE, 1U);
						DrawString((float)ScreenWidth() * 0.49f, (float)ScreenHeight() * 0.5f, std::to_string((int)(10-fCounter)), olc::WHITE, 4U);
					
						if (GetKey(olc::SPACE).bPressed && !bPlayer1)
						{
							Player* player = new Player(1);
							listPlayers.push_back(*player);
							bPlayer1 = true;
							bGame = true;
							fCounter = 0.0f;
							fTimer = 0;
							bTimeOut = false;
							dWorldPos = 0;
							listSpawns = listSpawnsBkp;
						}

						if (GetMouse(0).bPressed && !bPlayer2)
						{
							Player* player = new Player(2);
							listPlayers.push_back(*player);
							bPlayer2 = true;
							bGame = true;
							fCounter = 0.0f;
							fTimer = 0;
							bTimeOut = false;
							dWorldPos = 0;
							listSpawns = listSpawnsBkp;

						}
							
							if (fCounter > 10)	restart();
					}
			
				}
				

				//===========MENU============
				if (bPause)
				{
					if (bMainMenu) 
					{
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.28f, (bStart) ? "MAIN MENU" : "PAUSE!", olc::WHITE, 3U);

						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.42f, "Resume", (bResume) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.52f, "Restart", (bRestart) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.62f, "Options", (bOptions) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.72f, "Quit", (bQuit) ? magicPixel : olc::DARK_GREY, 2U);
					}
					if (bOptionsMenu)
					{
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.28f, "OPTIONS", olc::WHITE, 3U);

						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.42f, "Controls", (bInfo) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.52f, "Sounds", (bSounds) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.72f, "Back", (bBack) ? magicPixel : olc::DARK_GREY, 2U);
					}
					if (bInfoPanel)
					{

						DrawString((float)ScreenWidth() * 0.37f, (float)ScreenHeight() * 0.18f, "CONTROLS", olc::GREY, 3U);
						DrawString((float)ScreenWidth() * 0.13f, (float)ScreenHeight() * 0.34f, "PLAYER1", olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.08f, (float)ScreenHeight() * 0.44f, "W,S,A,D = Movement \n\nSPACE = Fire \n\n1, 2, 3 = Select Spells \n\nALT = Cast Spell", olc::DARK_GREY);

						DrawString((float)ScreenWidth() * 0.68f, (float)ScreenHeight() * 0.34f, "PLAYER2", olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.61f, (float)ScreenHeight() * 0.44f, "ARROW KEYS = Movement \n\nLEFT MOUSE BUTTON = Fire \n\nDEL, END, PGDN = Select Spells \n\nRIGHT MOUSE BUTTON = Cast Spell", olc::DARK_GREY);

						DrawString((float)ScreenWidth() * 0.46f, (float)ScreenHeight() * 0.72f, "Back", (bBack) ? magicPixel : olc::DARK_GREY, 2U);
					}
					if(bSoundPanel)
					{
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.28f, "SOUNDS", olc::WHITE, 3U);

						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.42f, "Volume", (bVolume) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.56f, (float)ScreenHeight() * 0.42f, std::to_string(nVolumeLvl * 10) + "%", (bVolume) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.52f, "Music", (bMusic) ? magicPixel : olc::DARK_GREY, 2U);
						DrawString((float)ScreenWidth() * 0.55f, (float)ScreenHeight() * 0.52f, (bMusicON) ? "ON" : "OFF", (bMusic) ? magicPixel : olc::DARK_GREY, 2U);

						DrawString((float)ScreenWidth() * 0.4f, (float)ScreenHeight() * 0.72f, "Back", (bBack) ? magicPixel : olc::DARK_GREY, 2U);
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
	if (demo.Construct(700, 350, 2, 2, true, true))
		demo.Start();
	return 0;
}