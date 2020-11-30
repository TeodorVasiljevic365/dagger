#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"

#include "gameplay/team_game/hero_controller.h"
#include "gameplay/team_game/hero_controller_fsm.h"

using namespace dagger;

// Idle


void HeroControllerFSM::Idle::Enter(HeroControllerFSM::StateComponent& state_)
{
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "chara_hero:hero_idle");
}

DEFAULT_EXIT(HeroControllerFSM, Idle);

void HeroControllerFSM::Idle::Run(HeroControllerFSM::StateComponent& state_)
{
	auto& input = Engine::Registry().get<InputReceiver>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("run-left-right")) || EPSILON_NOT_ZERO(input.Get("run-up-down")))
	{
		GoTo(EHeroStates::Running, state_);
	}
}


// Running

void HeroControllerFSM::Running::Enter(HeroControllerFSM::StateComponent& state_)
{
}

// same as: DEFAULT_EXIT(CharacterControllerFSM, Running);
void HeroControllerFSM::Running::Exit(HeroControllerFSM::StateComponent& state_)
{}

void HeroControllerFSM::Running::Run(HeroControllerFSM::StateComponent& state_)
{
	auto&& [sprite, input, character] = Engine::Registry().get<Sprite, InputReceiver, team_game::TeamGameCharacter>(state_.entity);
	auto& animator = Engine::Registry().get<Animator>(state_.entity);

	Float32 run_left_right = input.Get("run-left-right");
	Float32 run_up_down = input.Get("run-up-down");

	if (EPSILON_ZERO(run_left_right) && EPSILON_ZERO(run_up_down))
	{
		GoTo(EHeroStates::Idle, state_);
	}
	else
	{
		if (!EPSILON_ZERO(run_left_right))
		{
			sprite.scale.x = run_left_right * abs(sprite.scale.x);;
			sprite.position.x += character.speed * run_left_right * Engine::DeltaTime();
		}
		
		if (!EPSILON_ZERO(run_up_down))
		{
			sprite.position.y += character.speed * run_up_down * Engine::DeltaTime();
		}
		
	}
	if (run_up_down > 0)
		AnimatorPlay(animator, "chara_hero:hero_move_up");

	else if (run_up_down < 0)
		AnimatorPlay(animator, "chara_hero:hero_move_down");

	else if(run_left_right != 0)
		AnimatorPlay(animator, "chara_hero:hero_move_side");
}