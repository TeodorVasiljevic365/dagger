#include "tank_warfare_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/tank_warfare/tank_controller.h"
#include "gameplay/tank_warfare/camera_center.h"

using namespace dagger;
using namespace tank_warfare;

void TankWarfare::GameplaySystemsSetup(Engine &engine_)
{
    engine_.AddSystem<SimpleCollisionsSystem>();
    engine_.AddSystem<TankControllerSystem>();
    engine_.AddSystem<CameraCenterSystem>();
}

void TankWarfare::WorldSetup(Engine &engine_)
{
    tank_warfare::SetupCamera(engine_);
    tank_warfare::SetupTestWorld(engine_);
}

void tank_warfare::SetupCamera(Engine &engine_)
{
    ShaderSystem::Use("standard");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 2;
    camera->position = { 0, 0, 0 };
    camera->Update();

    auto& reg = engine_.Registry();
    auto entity = reg.create();
    auto& camParams = reg.emplace<CameraParams>(entity);
    camParams.camZoom = camera->zoom;
    camParams.camXY = camera->size;
}

void tank_warfare::SetupWorld(Engine &engine_)
{
    auto& reg = engine_.Registry();

    float zPos = 1.f;

    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "logos:dagger");
        float ratio = sprite.size.y / sprite.size.x;
        sprite.size = { 500 / ratio, 500  };

        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 0, 0, zPos };

        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size = sprite.size;
    }
}

void tank_warfare::SetupTestWorld(Engine& engine_)
{
    auto& reg = Engine::Registry();
    for (int i = -10; i < 10; i++)
    {
        for (int j = -10; j < 10; j++)
        {
            auto entity = reg.create();
            auto& sprite = reg.emplace<Sprite>(entity);
            AssignSprite(sprite, fmt::format("jovanovici:tile_map:tile_grass{}", 1 + (rand() % 3)));
            sprite.position = { i * 48, j * 48, 2 };
        }
    }
    
    //tank1
    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    auto& anim = reg.emplace<Animator>(entity);
    auto& transform = reg.emplace<Transform>(entity);
    auto& input = reg.emplace<InputReceiver>(entity);
    auto& tank = reg.emplace<TankCharacter>(entity);
    auto& cam = reg.emplace<CameraCenter>(entity);
    sprite.scale = { 1, 1 };
    sprite.position = { 0, 0, 1 };
    input.contexts.push_back("tank1");
    AssignSprite(sprite, "jovanovici:tank:tank3_side");

    //tank1
    auto entity1 = reg.create();
    auto& sprite1 = reg.emplace<Sprite>(entity1);
    auto& anim1 = reg.emplace<Animator>(entity1);
    auto& transform1 = reg.emplace<Transform>(entity1);
    auto& input1 = reg.emplace<InputReceiver>(entity1);
    auto& tank1 = reg.emplace<TankCharacter>(entity1);
    auto& cam1 = reg.emplace<CameraCenter>(entity1);
    sprite1.scale = { -1, 1 };
    sprite1.position = { 0, 0, 1 };
    input1.contexts.push_back("tank2");
    AssignSprite(sprite1, "jovanovici:tank:tank3_side");

}
