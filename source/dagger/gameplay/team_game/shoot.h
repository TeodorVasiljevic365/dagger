#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace lab
{
	struct Bullet {
		int damage = 15;
		Float32 speed = 100.f;
		Float32 speedX = 100.f;
		Float32 speedY = 100.f;
	};

	class ShootingSystem
		: public System
	{
	public:
		String SystemName() override {
			return "Character Controller System";
		}
		void Run() override;
	};
	void CreateBullet(Vector2 position, Float32 ratio, Vector2 directions);
}
