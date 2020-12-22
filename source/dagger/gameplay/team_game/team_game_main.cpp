#include "team_game_main.h"
#include "mage.h"
#include "golem.h"
#include "range_of_attack.h"
#include "towers.h"
#include "hp_system.h"
#include "controls.h"
#include "spell.h"
#include "game_manager.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"

#include "gameplay/common/simple_collisions.h"


using namespace dagger;
using namespace ancient_defenders;

void TeamGame::GameplaySystemsSetup(Engine &engine_)
{
      engine_.AddSystem<SimpleCollisionsSystem>();
      engine_.AddSystem<RangedTargetingSystem>();
      engine_.AddSystem<SpellBehaviorSystem>();
      engine_.AddSystem<MageBehaviorSystem>();
      engine_.AddSystem<EnemyBehaviorSystem>();
      engine_.AddSystem<TowerBehaviorSystem>();
      engine_.AddSystem<HealthManagementSystem>();
      engine_.AddSystem<PlayerControlsSystem>();
      engine_.AddSystem<GameManagerSystem>();
}

void TeamGame::WorldSetup(Engine &engine_)
{
    ShaderSystem::Use("standard");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    ancient_defenders::SetupWorld(engine_);
    ancient_defenders::LoadPath();
    ancient_defenders::LoadTowerSpots();
    ancient_defenders::SetupControls(engine_);
}

void ancient_defenders::SetupWorld(Engine &engine_)
{
    auto& reg = engine_.Registry();

    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "ancient_defenders:level1-ground");
        float ratio = sprite.size.y / sprite.size.x;
        sprite.size = { 800 , 600  };

        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 0, 0, 100 };
    }

    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "spritesheets:hp-bar:hp_BCK");
        sprite.scale = { 10,10 };
        
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 0, -260, 99 };
    }
    {
        auto player = new PlayerInfo();

        player->sprite = reg.create();
        
        auto& sprite = reg.emplace<Sprite>(player->sprite);

        AssignSprite(sprite, "spritesheets:hp-bar:hp_100");
        sprite.scale = { 10,10 };

        auto& transform = reg.emplace<Transform>(player->sprite);
        transform.position = { 0, -260, 99 };

        Engine::PutDefaultResource<PlayerInfo>(player);
    }

}



void ancient_defenders::SetupControls(Engine& engine_) {
    auto& reg = engine_.Registry();

    {
        auto entity = reg.create();
        auto & inputs = reg.emplace<InputReceiver>(entity);

        inputs.contexts.push_back("AD_Controls");
    }
}

void ancient_defenders::LoadPath() {

    FileInputStream inFile{ "path.txt" };

    Vector2 point;

    while (inFile >> point.x >> point.y) {
       WalkingPath::path.emplace_back(point);
    }

    WalkingPath::numberOfPoints = WalkingPath::path.size();
}

void ancient_defenders::LoadTowerSpots() {

    FileInputStream inFile{ "spots.txt" };

    Vector2 point;

    while (inFile >> point.x >> point.y) {
        TowerPlacementInfo::spotCoordinates.emplace_back(point);
    }
}