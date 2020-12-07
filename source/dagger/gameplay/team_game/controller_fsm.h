#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"
#include "gameplay/team_game/animation_fsm.h"

namespace team_game
{

	enum class ECharacterStates
	{
		Idle,
		Running,
		Dashing,
		Jumping,
		Falling,
		DoubleJumping,
		Using,
		Attacking,
		Tossing,
		Dead
	};

	struct CharacterControllerFSM : public FSM<ECharacterStates>
	{
		AnimationFSM animationFSM;

		DEFINE_STATE(CharacterControllerFSM, ECharacterStates, Idle);
		DEFINE_STATE(CharacterControllerFSM, ECharacterStates, Running);

		CharacterControllerFSM()
		{
			CONNECT_STATE(ECharacterStates, Idle);
			CONNECT_STATE(ECharacterStates, Running);
		}

		void Run(StateComponent& component_) override;
	};
}