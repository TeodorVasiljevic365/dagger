#include "team_game_main.h"
#include "mage.h"

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
    engine_.AddSystem<MageBehaviorSystem>();
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

    //ancient_defenders::SetupWorld(engine_);
    ancient_defenders::SetupDemoCharacter(engine_);
}

void ancient_defenders::SetupWorld(Engine &engine_)
{
    auto& reg = engine_.Registry();

    float zPos = 1.f;

    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSpriteTexture(sprite, "logos:dagger");
        float ratio = sprite.size.y / sprite.size.x;
        sprite.size = { 500 / ratio, 500  };

        auto& transform = reg.emplace<Transform>(entity);
        transform.position = { 0, 0, zPos };

        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size = sprite.size;
    }
}

struct Mage {
    Entity entity;
    Sprite& sprite;
    Transform& coordinates;
    Animator& animator;
    MageStats& mage;

    static Mage Get(Entity entity)
    {
        auto& reg = Engine::Registry();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& pos = reg.get_or_emplace<Transform>(entity);
        auto& anim = reg.get_or_emplace<Animator>(entity);
        auto& mag = reg.get_or_emplace<MageStats>(entity);
        return Mage{ entity, sprite, pos, anim, mag };
    }

    static Mage Create()
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto mag = Mage::Get(entity);

        AssignSpriteTexture(mag.sprite, "ancient_defenders:mage");
        float ratio = mag.sprite.size.y / mag.sprite.size.x;
        mag.sprite.size = { 200 / ratio, 200 };

        mag.coordinates.position = { 300, 0, 1.0f };
        
        mag.mage.speed = 50.0f;

        return mag;
    }


};

void ancient_defenders::SetupDemoCharacter(Engine& engine_) {
    auto& reg = engine_.Registry();

    float zPos = 1.f;

    auto demoMage = Mage::Create();
}