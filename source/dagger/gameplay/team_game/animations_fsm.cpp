#include "animations_fsm.h"
#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "gameplay/team_game/brawler_controller.h"
#include "gameplay/team_game/physics.h"

using namespace dagger;
using namespace team_game;

void AnimationsFSM::Idle::Enter(AnimationsFSM::StateComponent& state_) {
    auto& animator_ = Engine::Registry().get<Animator>(state_.entity);
    AnimatorPlay(animator_, "character:IDLE");
}
DEFAULT_RUN(AnimationsFSM, Idle);
DEFAULT_EXIT(AnimationsFSM, Idle);

void AnimationsFSM::Running::Enter(AnimationsFSM::StateComponent& state_) {
    auto& animator_ = Engine::Registry().get<Animator>(state_.entity);
    AnimatorPlay(animator_, "character:RUN");
}

DEFAULT_RUN(AnimationsFSM, Running);
DEFAULT_EXIT(AnimationsFSM, Running);

void AnimationsFSM::Jumping::Enter(AnimationsFSM::StateComponent& state_) 
{
    auto& animator_ = Engine::Registry().get<Animator>(state_.entity);
    AnimatorPlay(animator_, "character:JUMP");
}
DEFAULT_RUN(AnimationsFSM, Jumping);

DEFAULT_EXIT(AnimationsFSM, Jumping);

void AnimationsFSM::Falling::Enter(AnimationsFSM::StateComponent& state_) 
{
    auto& animator_ = Engine::Registry().get<Animator>(state_.entity);
    AnimatorPlay(animator_, "character:FALL");
}
DEFAULT_RUN(AnimationsFSM, Falling);
DEFAULT_EXIT(AnimationsFSM, Falling);
