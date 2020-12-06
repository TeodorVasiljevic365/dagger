#pragma once
#include "core/core.h"
#include "core/game/finite_state_machine.h"

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
		DEFINE_STATE(CharacterControllerFSM, ECharacterStates, Idle);
		DEFINE_STATE(CharacterControllerFSM, ECharacterStates, Running);
		DEFINE_STATE(CharacterControllerFSM, ECharacterStates, Jumping);

		CharacterControllerFSM()
		{
			CONNECT_STATE(ECharacterStates, Idle);
			CONNECT_STATE(ECharacterStates, Running);
			CONNECT_STATE(ECharacterStates, Jumping);
		}
	};
}