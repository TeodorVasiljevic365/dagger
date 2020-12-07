#pragma once

#include "mage.h"
#include "range_of_attack.h"

#include "core/core.h"
#include "core/system.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"

#include "gameplay/common/simple_collisions.h"

using namespace dagger;

namespace ancient_defenders {

	struct GolemStats {
		Float32 health;
		Float32 speed;
		Float32 meleeDmg;

		SInt32 postition = (SInt32) WalkingPath::numberOfPoints - 1;
		Vector2 direction = { -1, 0 };

		EAction currentAction = EAction::Moving;
	};

	struct Golem {
		Entity entity;
		Sprite& sprite;
		Transform& coordinates;
		Animator& animator;
		GolemStats& golem;
		SimpleCollision& hitbox;
		RangeOfAttack& range;

		static Golem Get(Entity entity_);

		static Golem Create();

	};

	class GolemBehaviorSystem
		: public System
	{

	public:
		inline String SystemName() { return "Racing Tools System"; }

		void SpinUp() override;
		void WindDown() override;
		void Run() override;

	private:
		void OnEndOfFrame();
	};

}