#pragma once

#include "core/core.h"
#include "core/system.h"
#include "gameplay/team_game/controller_fsm.h"

using namespace dagger;

namespace team_game
{
	struct PlayerCharacter
	{
		float speed{ 140.f };
	};

	class CharacterControllerSystem
		: public System
	{
		CharacterControllerFSM characterFSM;

	public:
		String SystemName() override {
			return "Character Controller System";
		}

		void Run() override;
	};
}
