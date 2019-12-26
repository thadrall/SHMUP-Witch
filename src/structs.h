#pragma once



olc::Sprite* sprPlayer;
olc::Sprite* sprPlayerBullet;
olc::Sprite* sprHealth;
olc::Sprite* sprShield;
olc::Sprite* sprTree;
olc::Sprite* sprSky;
olc::Sprite* sprMountains;
olc::Sprite* sprGameOver;
olc::Sprite* sprStart;
olc::vf2d vSkyPos;
olc::Sprite* sprEnemy[5];
olc::Sprite* sprEnemyBullet[2];



float m; ///<----- WTF?!?!?!

float fWorldSpeed = 90.0f;
double dWorldPos = 0.0f;
float fCounter = 0.0f;
std::array<olc::vf2d, 400> aryStars;
std::array<olc::vf2d, 18> aryTree;

class Player {
public:

	olc::vf2d pos;
	float fSpeed;
	float fGunReloadTimer;
	float fFireRate;
	bool bCanFire;


	int nLives;
	int nScore;


	Player()
	{
		pos = { 400, 200 };
		fSpeed = 200.0f;
		fGunReloadTimer = 0.0f;
		fFireRate = 0.2f;
		bCanFire = false;
		nLives = 3;
		nScore = 0;
	};


};


struct sBullet
{
	olc::vf2d pos;
	olc::vf2d vel;
	bool remove = false;
	uint32_t nBulletTypeID = 0;

};

struct sEnemy;

struct sEnemyDefinition
{
	double dTriggerTime = 0.0f;
	uint32_t nSpriteID = 0;
	int nEnemyLives = 0;
	float fOffset = 0.0f;

	std::function<void(sEnemy&, float, float, Player&)> funcMove;
	std::function<void(sEnemy&, float, float, std::list<sBullet>&, Player&)> funcFire;


	uint32_t nBulletTypeID = 0;
	int nScore = 0;
};

struct sEnemy
{
	olc::vf2d pos;
	sEnemyDefinition def;

	std::array<float, 4> dataMove{ 0 };
	std::array<float, 4> dataFire{ 0 };

	void Update(float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, Player& p)
	{
		def.funcMove(*this, fElapseTime, fScrollSpeed, p);
		def.funcFire(*this, fElapseTime, fScrollSpeed, bullets, p);

	}
};

olc::vf2d GetMiddle(const olc::Sprite* s)
{
	return { (float)s->width / 2.0f, (float)s->height / 2.0f };
}