#pragma once

olc::Sprite* sprPlayer[2];
olc::Sprite* sprPlayerBullet[2];
olc::Sprite* sprHealth[2];
olc::Sprite* sprShield[2];
olc::Sprite* sprTree;
olc::Sprite* sprSky[2];
olc::Sprite* sprMountains[2];
olc::Sprite* sprGameOver;
olc::Sprite* sprStart;
olc::vf2d vSkyPos;
olc::Sprite* sprEnemy[5];
olc::Sprite* sprEnemyBullet[2];


float fWorldSpeed = 90.0f;
double dWorldPos = 0.0f;
float fCounter = 0.0f;
std::array<olc::vf2d, 400> aryStars;
std::array<olc::vf2d, 18> aryTree;
std::array<olc::vf2d, 4> arySky;
std::array<olc::vf2d, 2> aryMountains;


struct sBullet;

class Player {
public:

	olc::vf2d pos;
	float fSpeed;
	float fGunReloadTimer;
	float fFireRate;
	float fProtectionTimer;
	float fCooldown;
	bool bIsProtected;
	bool bCanFire;
	bool bIsFire;
	bool bDead;
	int nLives;
	int nScore;
	uint32_t nPlayerID;

	std::list<sBullet> listPlayerBullet;

	Player(uint32_t ID)
	{
		pos = { 200, 100 };
		fSpeed = 200.0f;
		fGunReloadTimer = 0.0f;
		fCooldown = 2.5f;
		fProtectionTimer = 0.0f;
		fFireRate = 0.12f;
		bIsFire = false;
		bCanFire = false;
		bIsProtected = true;
		nLives = 3;
		nScore = 0;
		nPlayerID = ID;
		listPlayerBullet.clear();
		bDead = false;
	}

	~Player() = default;
};


struct sBullet
{
	olc::vf2d pos;
	olc::vf2d vel;
	bool remove = false;
	uint32_t nBulletTypeID = 0;
	uint32_t nBulletPlayerID = 0;

};

struct sEnemy;

struct sEnemyDefinition
{
	double dTriggerTime = 0.0f;
	uint32_t nSpriteID = 0;
	int nEnemyLives = 0;
	float fOffset = 0.0f;

	std::function<void(sEnemy&, float, float, olc::vf2d)> funcMove;
	std::function<void(sEnemy&, float, float, std::list<sBullet>&, olc::vf2d)> funcFire;


	uint32_t nBulletTypeID = 0;
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