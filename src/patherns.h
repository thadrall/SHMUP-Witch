/*======================================

This file contain move and shot patherns

=======================================*/

#pragma once
#include "olcPixelGameEngine.h"
#include <array>
#include "structs.h"


// Movement patterns
auto Move_None = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime;
};

auto Move_Moon = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime;
	if (e.pos.x <= vScreenSize.x * 0.75f) e.pos.x = vScreenSize.x * 0.75f;
};

auto Move_Fast = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime * 5.0f;
};

auto Move_Sin = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime * 1.5f;
	e.dataMove[0] += fElapseTime * 1.5f;
	e.pos.y += 30.f * cosf(e.dataMove[0]) * 3 * fElapseTime;
};

auto Move_Sin_Fast = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime * 5.5f;
	e.dataMove[0] += fElapseTime * 2.5f;
	if (e.pos.y > p.y) e.pos.y -= 120 * fElapseTime;
	if (e.pos.y < p.y) e.pos.y += 120 * fElapseTime;
};

auto Move_Bat = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, olc::vf2d p)
{
	e.pos.x -= fScrollSpeed * fElapseTime * 1.5f;
	float fRotationSpeed = 1.5f;
	e.dataMove[0] += fElapseTime;
	e.pos.y += 400.0f * sinf(e.dataMove[0] * (e.def.fOffset) * 30) * fElapseTime;
	e.pos.x += cosf(e.dataMove[0] * 20) * 90 * fElapseTime;



};

// Fire Patterns
auto Fire_None = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
{
};

auto Fire_Straight = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
{
	constexpr float fDelay = 0.8f;
	e.dataFire[0] += fElapseTime;
	if (e.dataFire[0] >= fDelay)
	{
		e.dataFire[0] -= fDelay;
		sBullet b;
		b.nBulletTypeID = 0;
		b.pos = e.pos + olc::vf2d(-(float)sprEnemy[e.def.nSpriteID]->width / 2, ((float)sprEnemy[e.def.nSpriteID]->height / 2) - ((float)sprEnemyBullet[b.nBulletTypeID]->height / 2));
		b.vel = {p - e.pos };
		bullets.push_back(b);
	}
};

auto Fire_Fast = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
{
	constexpr float fDelay = 0.4f;
	e.dataFire[0] += fElapseTime;
	e.dataFire[1] += fElapseTime;
	if (e.dataFire[0] >= fDelay)
	{
		e.dataFire[0] -= fDelay;
		sBullet b;
		b.nBulletTypeID = 1;
		b.pos = e.pos + olc::vf2d(-(float)sprEnemy[e.def.nSpriteID]->width / 2, ((float)sprEnemy[e.def.nSpriteID]->height / 2) - ((float)sprEnemyBullet[b.nBulletTypeID]->height / 2));
		b.vel = { -850.0f, 0.0f };
		bullets.push_back(b);
	}
};

auto Fire_Triple = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
{
	constexpr float fDelay = 1.2f;
	constexpr int nBullets = 3;
	constexpr float fTheta = 3.14159f * 2.0f / (float)nBullets;
	e.dataFire[0] += fElapseTime;
	if (e.dataFire[0] >= fDelay)
	{
		e.dataFire[0] -= fDelay;

		for (int i = 0; i < nBullets; i++)
		{
			sBullet b;
			b.nBulletTypeID = 0;
			b.pos = e.pos + olc::vf2d(-(float)sprEnemy[e.def.nSpriteID]->width / 2, ((float)sprEnemy[e.def.nSpriteID]->height / 2) - ((float)sprEnemyBullet[b.nBulletTypeID]->height / 2));
			b.vel = { -180.0f + cos(fTheta), -45.0f + (45.0f * i) };
			bullets.push_back(b);
		}
	}
};


auto Fire_Moon = [&](sEnemy& e, float fElapseTime, float fScrollSpeed, std::list<sBullet>& bullets, olc::vf2d p)
{
	constexpr float fDelay = 1.1f;
	constexpr int nBullets = 15;
	constexpr float fTheta = 3.14159f * 2.0f / (float)nBullets;

	e.dataFire[1] += fElapseTime;
	if (e.dataFire[1] >= fDelay)
	{
		e.dataFire[1] -= fDelay;

			sBullet b;
			b.nBulletTypeID = 1;
			b.pos = e.pos + olc::vf2d(0, ((float)sprEnemy[e.def.nSpriteID]->height / 2.0f));
			b.vel = { p - (e.pos + olc::vf2d(0 , ((float)sprEnemy[e.def.nSpriteID]->height / 2.0f))) };
			bullets.push_back(b);
	}
	constexpr float fDelay2 = 1.1f;
	constexpr int nBullets2 = 6;
	constexpr float fTheta2 = 2.0f * 3.14159f / (float)nBullets2;
	e.dataFire[2] += fElapseTime;
	if (e.dataFire[2] >= fDelay2)
	{
		e.dataFire[2] -= fDelay2;
		for (int i = 0; i < nBullets2; i++)
		{
			sBullet b;
			b.nBulletTypeID = 0;
			b.pos = e.pos + olc::vf2d(-10 , ((float)sprEnemy[e.def.nSpriteID]->height / 2.0f));
			b.vel = { 180.0f * cosf(fTheta * i+46), 180.0f * sinf(fTheta * i+46) };
			bullets.push_back(b);
		}
	}
};