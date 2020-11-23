#include "plight.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "tools/diagnostics.h"

#include "gameplay/plight/topdown_controller.h"
#include "gameplay/plight/plight_combat.h"
#include "gameplay/plight/plight_collisions.h"


using namespace dagger;
using namespace plight;

struct PlightCharacter
{
    Entity entity;
    Sprite& sprite;
    Animator& animator;
    InputReceiver& input;
    TopdownCharacter& character;
    PlightCollision& col;
    Transform& transform;
    CombatStats& cstats;

    static PlightCharacter Get(Entity entity)
    {
        auto& reg = Engine::Registry();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& anim = reg.get_or_emplace<Animator>(entity);
        auto& input = reg.get_or_emplace<InputReceiver>(entity);
        auto& character = reg.get_or_emplace<TopdownCharacter>(entity);
        auto& col = reg.get_or_emplace<PlightCollision>(entity);
        auto& transform = reg.get_or_emplace<Transform>(entity);
        auto& cstats = reg.get_or_emplace<CombatStats>(entity);
        return PlightCharacter{ entity, sprite, anim, input, character ,col,transform,cstats};
    }

    static PlightCharacter Create(
        String input_ = "",
        ColorRGB color_ = { 1, 1, 1 },
        Vector2 position_ = { 0, 0 })
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto chr = PlightCharacter::Get(entity);

        chr.sprite.scale = { 1, 1 };
        chr.sprite.position = { position_, 0.0f };
        chr.sprite.color = { color_, 1.0f };

        chr.col.size.x = 16;
        chr.col.size.y = 16;

        chr.transform.position = { position_, 0.0f };

        AssignSpriteTexture(chr.sprite, "Plight:big_deamon:IDLE:big_demon_idle_anim_f0");
        AnimatorPlay(chr.animator, "Plight:big_deamon:IDLE");

        if (input_ != "")
            chr.input.contexts.push_back(input_);

        chr.character.speed = 50;

        return chr;
    }
};

void Plight::GameplaySystemsSetup(Engine &engine_)
{
    engine_.AddSystem<TopdownControllerSystem>();
    engine_.AddSystem<PlightCollisionsSystem>();
    engine_.AddSystem<PlightCombatSystem>();

}

void Plight::WorldSetup(Engine &engine_)
{
    ShaderSystem::Use("standard");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    plight::SetupWorld_test1(engine_);
}

void plight::SetupWorld(Engine &engine_)
{
}

void setUpBackground(Engine& engine_) {
    auto& reg = engine_.Registry();
    for (int i = -50; i < 50; i++)
    {
        for (int j = -50; j < 50; j++)
        {
            auto entity = reg.create();
            auto& sprite = reg.emplace<Sprite>(entity);
            AssignSpriteTexture(sprite, fmt::format("Plight:floor:floor_{}", 1 + (rand() % 8)));
            sprite.position = { i * 16, j * 16, 10 };
        }
    }

}

void plight::SetupWorld_test1(Engine& engine_) {
   
    setUpBackground(engine_);

    auto mainChar = PlightCharacter::Create("ASDW_topdown", { 1, 1, 1 }, { -100, 0 });

    auto backgroundBar1 = Engine::Registry().create();
    auto currentHealthBar1 = Engine::Registry().create();

    mainChar.cstats.backgroundHealthBar = backgroundBar1;
    mainChar.cstats.currentHealthBar = currentHealthBar1;

    auto& backgroundSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.backgroundHealthBar);

    AssignSpriteTexture(backgroundSprite, "EmptyWhitePixel");
    backgroundSprite.color = { 0, 0, 0, 1 };
    backgroundSprite.size = { 50, 5 };
    backgroundSprite.scale = { 1, 1 };
    backgroundSprite.position = { -100, 125, 2 };

    auto& frontSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.currentHealthBar);

    AssignSpriteTexture(frontSprite, "EmptyWhitePixel");
    frontSprite.color = { 1, 0, 0, 1 };
    frontSprite.size = { 50, 5 };
    frontSprite.scale = { 1, 1 };
    frontSprite.position = { -100, 125, 1 };

   

    auto sndChar = PlightCharacter::Create("arrows_topdown", { 1, 0, 0 }, { 100, 0 });

    auto backgroundBar2 = Engine::Registry().create();
    auto currentHealthBar2 = Engine::Registry().create();

    sndChar.cstats.backgroundHealthBar = backgroundBar2;
    sndChar.cstats.currentHealthBar = currentHealthBar2;

    auto& backgroundSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.backgroundHealthBar);

    AssignSpriteTexture(backgroundSprite2, "EmptyWhitePixel");
    backgroundSprite2.color = { 0, 0, 0, 1 };
    backgroundSprite2.size = { 50, 5 };
    backgroundSprite2.scale = { 1, 1 };
    backgroundSprite2.position = { 100, 125, 2 };

    auto& frontSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.currentHealthBar);

    AssignSpriteTexture(frontSprite2, "EmptyWhitePixel");
    frontSprite2.color = { 1, 0, 0, 1 };
    frontSprite2.size = { 50, 5 };
    frontSprite2.scale = { 1, 1 };
    frontSprite2.position = { 100, 125, 1 };

}



