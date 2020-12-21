#include "controller_fsm.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include "gravity.h"

#include "gameplay/team_game/character_controller.h"
#include "animation_fsm.h"
#include "core/graphics/animations.h"
#include "gameplay/team_game/team_game_collisions.h"


using namespace dagger;

void team_game::CharacterControllerFSM::Run(StateComponent& component_)
{
	FSM<ECharacterStates>::Run(component_);
	auto& anim = Engine::Registry().get<AnimationFSM::StateComponent>(component_.entity);
	animationFSM.Run(anim);
}



void team_game::CharacterControllerFSM::Idle::Enter(CharacterControllerFSM::StateComponent& state_)
{
	auto& anim = Engine::Registry().get<AnimationFSM::StateComponent>(state_.entity);
	((CharacterControllerFSM*)this->GetFSM())->animationFSM.GoTo(EAnimationStates::Idle, anim);
}

DEFAULT_EXIT(team_game::CharacterControllerFSM, Idle);

void team_game::CharacterControllerFSM::Idle::Run(CharacterControllerFSM::StateComponent& state_)
{
	auto&& [input, gravity] = Engine::Registry().get<InputReceiver, Gravity>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("run")))
	{
		GoTo(ECharacterStates::Running, state_);
	}
	if (EPSILON_NOT_ZERO(input.Get("jump")))
	{
		gravity.verticalCurrentSpeed = gravity.verticalInitialSpeed;
		GoTo(ECharacterStates::InAir, state_);
	}
}



void team_game::CharacterControllerFSM::Running::Enter(CharacterControllerFSM::StateComponent& state_)
{
	auto& anim = Engine::Registry().get<AnimationFSM::StateComponent>(state_.entity);
	((CharacterControllerFSM*)this->GetFSM())->animationFSM.GoTo(EAnimationStates::Running, anim);
}

void team_game::CharacterControllerFSM::Running::Exit(CharacterControllerFSM::StateComponent& state_)
{}

void team_game::CharacterControllerFSM::Running::Run(CharacterControllerFSM::StateComponent& state_)
{
	auto&& [sprite, transform, input, character, collider, gravity] = Engine::Registry().get<Sprite, Transform, InputReceiver, team_game::PlayerCharacter, Collider, Gravity>(state_.entity);

	Float32 run = input.Get("run");
	Float32 jump = input.Get("jump");

	if (EPSILON_ZERO(jump) && !collider.canGoDown)
	{
		if (EPSILON_ZERO(run))
		{
			GoTo(ECharacterStates::Idle, state_);
		}
		else
		{
			sprite.scale.x = run;
			transform.position.x += character.speed * sprite.scale.x * Engine::DeltaTime();
		}
	}
	else
	{
		if (!EPSILON_ZERO(jump))
		{
			gravity.verticalCurrentSpeed = gravity.verticalInitialSpeed;
		}
		GoTo(ECharacterStates::InAir, state_);
	}
	
}



void team_game::CharacterControllerFSM::InAir::Enter(CharacterControllerFSM::StateComponent& state_)
{
	auto&& [input, character, gravity, anim] = Engine::Registry().get<InputReceiver, team_game::PlayerCharacter, Gravity, AnimationFSM::StateComponent>(state_.entity);
	((CharacterControllerFSM*)this->GetFSM())->animationFSM.GoTo(EAnimationStates::Jumping, anim);
	
}

void team_game::CharacterControllerFSM::InAir::Exit(CharacterControllerFSM::StateComponent& state_)
{
	auto&& [sprite, transform, input, character, collider, gravity] = Engine::Registry().get<Sprite, Transform, InputReceiver, team_game::PlayerCharacter, Collider, Gravity>(state_.entity);
	gravity.verticalCurrentSpeed = 0.f;
}

void team_game::CharacterControllerFSM::InAir::Run(CharacterControllerFSM::StateComponent& state_)
{
	auto&& [sprite, transform, input, character, collider, gravity] = Engine::Registry().get<Sprite, Transform, InputReceiver, team_game::PlayerCharacter, Collider, Gravity>(state_.entity);

	Float32 run = input.Get("run");
	Float32 jump = input.Get("jump");

	if (EPSILON_NOT_ZERO(run))
	{	
		sprite.scale.x = run;
		transform.position.x += character.speed * sprite.scale.x * Engine::DeltaTime();
	}

	if (!collider.canGoDown)
	{
		GoTo(ECharacterStates::Idle, state_);
	}
}

