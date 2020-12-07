#include "team_game_main.h"

#include <fstream>

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/animations.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"

#include "gameplay/team_game/game_manager.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/team_game/character_controller.h"
#include "gameplay/team_game/team_game_player_input.h"

using namespace dagger;

void team_game::TeamGame::GameplaySystemsSetup(Engine &engine_)
{
    engine_.AddSystem<GameManagerSystem>();
    engine_.AddSystem<SimpleCollisionsSystem>();
    engine_.AddSystem<CharacterControllerSystem>();
    //engine_.AddSystem<TeamGamePlayerInputSystem>();
}

void team_game::TeamGame::WorldSetup(Engine &engine_)
{
    ShaderSystem::Use("standard");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    String filePath = "levels/player_positions.txt";
    FilePath path{ filePath };
    std::ifstream fin(Files::absolute(path).string().c_str());
    Vector3 playerPos;

    while (fin >> playerPos.x >> playerPos.y >> playerPos.z)
    {
        GameManagerSystem::GetPlayerPositionsPerLevel().push_back(playerPos);
    }
}

void team_game::SetupWorld(Engine& engine_)
{
    auto& reg = engine_.Registry();
    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "TeamGame:Characters:Player-Bomb_Guy:Idle:1");
        sprite.position = GameManagerSystem::GetPlayerPositionsPerLevel()[GameManagerSystem::GetCurrentLevel()-1];

        auto& input = reg.emplace<InputReceiver>(entity);
        input.contexts.push_back("Controls");

        auto& animation = reg.emplace<Animator>(entity);
//        AnimatorPlay(animation, "TeamGame:Player-Bomb_Guy:RUNNING");
        ATTACH_TO_FSM(team_game::AnimationFSM, entity);

        auto& character = reg.emplace<PlayerCharacter>(entity);
        character.speed = 50;
        ATTACH_TO_FSM(team_game::CharacterControllerFSM, entity);
    }
}
