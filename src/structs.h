/*========================================
		Structs and definitions
=========================================*/

#pragma once

olc::Sprite* sprPlayer[2];
olc::Sprite* sprPlayerAtack[2];
olc::Sprite* sprPlayerBullet[4];
olc::Sprite* sprHealth[2];
olc::Sprite* sprShield[2];
olc::Sprite* sprTree;
olc::Sprite* sprSky[2];
olc::Sprite* sprMountains[2];
olc::Sprite* sprGameOver;
olc::Sprite* sprStart;
olc::Sprite* sprEnemy[5];
olc::Sprite* sprEnemyBullet[2];
olc::Sprite* sprSplash[2];
olc::Sprite* sprPowerUps[3];
olc::Sprite* sprTestAnimation[4];
olc::Sprite* sprManaFrame[2];
olc::Sprite* sprWave[2];
olc::Sprite* sprBoost[4];
olc::Sprite* sprUISpecialA[6];
olc::Sprite* sprUISpecialD[6];
olc::Sprite* sprSpeedBoost[2];
olc::Sprite* sprBossBar[2];


olc::vf2d vSkyPos;
olc::vf2d vScreenSize;

float fWorldSpeed = 90.0f;
double dWorldPos = 0.0f;
float fCounter = 0.0f;
std::array<olc::vf2d, 400> aryStars;
std::array<olc::vf2d, 18> aryTree;
std::array<olc::vf2d, 4> arySky;
std::array<olc::vf2d, 2> aryMountains;


struct sBullet;
struct sSpell;
struct sBoost;
struct sEnemy;

class Player {
public:

	//Player characteristics
	olc::vf2d pos;
	olc::vf2d vel;
	float fSpeed;
	bool bDead;
	int nLives;
	int nScore;
	uint32_t nPlayerID;

	//INPUT
	olc::vf2d vInput;
	float fInputSens;
	bool bInput = false;
	olc::vf2d vFinalVel;

	//Fire
	float fGunReloadTimer;
	float fFireRate;
	float fBulletSpeed;
	bool bCanFire;
	bool bIsFire;
	std::list<sBullet> listPlayerBullet;

	//SPECIAL
	uint32_t nActiveSpecialID = 1;
	bool bActiveSpecial;

	//Shield
	float fProtectionTimer;
	float fShieldDuration;
	bool bIsProtected;

	//Mana
	float fMana; 
	float fMaxMana;
	float fManaRegeneration;

	//Spells
	bool bSpecial; 
	float fSpecialManaCost;
	float fSpecialTimer;
	float fSpeciallCooldown;
	bool bSpecialReady;
	std::list<sSpell> listPlayerSpell;

	//Boost
	bool bBoost;
	bool bBoostActive;
	float fBoostBuffor;
	std::list<sBoost>listPlayerBoost;

	//Animations counter
	float fAnimationFrame;


	Player(uint32_t ID)
	{
		pos = { 200, 100 };
		vel = { 200, 100 };
		vInput = { 200, 100 };
		fSpeed = 220.0f;
		fInputSens = 7;
		fGunReloadTimer = 0.0f;
		fBulletSpeed = 700;
		fShieldDuration = 2.5f;
		fMana = 40.0f;
		fMaxMana = 100.0f;
		fManaRegeneration = 3;
		fSpeciallCooldown = 1;
		fSpecialManaCost = 50.0f;
		fProtectionTimer = 0.0f;
		fFireRate = 0.17f;
		bIsFire = false;
		bCanFire = false;
		bIsProtected = true;
		nLives = 3;
		nScore = 0;
		nPlayerID = ID;
		listPlayerBullet.clear();
		listPlayerSpell.clear();
		listPlayerBoost.clear();
		bDead = false;
	}

	~Player() = default;

	olc::vf2d GetPos(){ return pos; }
};

struct sAnimation {

	float fTimer = 0.0f;
	std::list<olc::Sprite*> listFrame;
	void loadSprites(olc::Sprite* sprite[], int nSprites) 
	{
		for (int i = 0; i < nSprites; i++)
		{
			listFrame.push_back(sprite[i]);
		}
	}
	int nCurrentFrame = 0;

	olc::Sprite* playAnimation(float fElapsedTime, float fFrameDuration = 0.3f)
	{

		
		fTimer += fElapsedTime;
		if (fTimer > fFrameDuration)
		{
			fTimer = 0.0f;
			nCurrentFrame++;
			if (nCurrentFrame >= listFrame.size()) nCurrentFrame = 0;
		}
		return* std::next(listFrame.begin(), nCurrentFrame);
	}
};

struct sDrops
{
	olc::vf2d pos;
	float fCounter = 0.0f;
	float fFrameCounter = 0.0f;
	uint32_t nID;
	bool bRemove = false;
};

struct sSplash
{
	olc::vf2d pos;
	float fCounter;
	uint32_t nID;
};

struct sBullet
{
	olc::vf2d pos;
	olc::vf2d vel;
	bool bRemove = false;
	uint32_t nBulletTypeID = 0;
	float fCounter;

};

struct sSpell
{
	olc::vf2d pos;
	olc::vf2d vel;
	bool bReady = false;
	float fCounter = 0.0f;
	float fSpellSpeed = 520;
	float fSpellCooldown = 0.02f;

};

struct sBoost
{	
	olc::vf2d offset;
	uint32_t nBoostID;
	float fDuration;
	float fBonusValue;
	float fBoostCounter = 0.0f;
	float fCharge = 0.0f;
	float fManaCost;
	bool bReady;
	bool bRemove = false;

};


struct sEnemyDefinition
{
	int dTriggerTime = 0;
	float fOffset = 0.0f;

	uint32_t nSpriteID = 0;
	bool bBoos = 0;
	int nEnemyLives = 0;
	

	std::function<void(sEnemy&, float, float, olc::vf2d)> funcMove;
	std::function<void(sEnemy&, float, float, std::list<sBullet>&, olc::vf2d)> funcFire;

	int nDropRate;
	int nScore = 0;
};

struct sEnemy
{
	olc::vf2d pos;
	sEnemyDefinition def;

	std::array<float, 4> dataMove{ 0 };
	std::array<float, 4> dataFire{ 0 };

	void Update(float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
	{
		def.funcMove(*this, fElapseTime, fScrollSpeed, p);
		def.funcFire(*this, fElapseTime, fScrollSpeed, bullets, p);

	}
};

olc::vf2d GetMiddle(const olc::Sprite* s)
{
	return { (float)s->width / 2.0f, (float)s->height / 2.0f };
}