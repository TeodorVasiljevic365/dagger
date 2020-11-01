#include "character_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"

using namespace example1;

void CharacterControllerSystem::OnInitialize(Registry& registry_, Entity entity_)
{
    InputReceiver& receiver = registry_.get<InputReceiver>(entity_);
    for (auto command : { "run", "jump", "down", "heavy", "light", "use" })
    {
        receiver.values[command] = 0;
    }
}

void CharacterControllerSystem::SpinUp()
{
    Engine::Registry().on_construct<InputReceiver>().connect<&CharacterControllerSystem::OnInitialize>(this);
}

void CharacterControllerSystem::Run()
{
    Engine::Registry().view<InputReceiver, Sprite, Animator>().each(
        [](const InputReceiver input_, Sprite& sprite_, Animator& animator_)
        {
            Float32 run = input_.values.at("run");
            if (run == 0)
            {
                AnimatorPlay(animator_, "souls_like_knight_character:IDLE");
            }
            else
            {
                AnimatorPlay(animator_, "souls_like_knight_character:RUN");
                sprite_.scale.x = run;
                sprite_.position.x += sprite_.scale.x * Engine::DeltaTime();

                auto cam = Engine::GetDefaultResource<Camera>();
                cam->zoom += 0.01f * run;
                Engine::Dispatcher().trigger<Camera>(*cam);
            }
        });
}

void CharacterControllerSystem::WindDown()
{
    Engine::Registry().on_construct<InputReceiver>().disconnect<&CharacterControllerSystem::OnInitialize>(this);
}


void example1::SetupSystems(Engine& engine_)
{
    engine_.AddSystem<CharacterControllerSystem>();
}

void example1::SetupWorld(Engine& engine_)
{
    ShaderSystem::Use("standard");

    Camera camera;
    camera.mode = ECameraMode::FixedResolution;
    camera.size = { 800, 600 };
    Engine::Dispatcher().trigger<Camera>(camera);

    auto& reg = engine_.GetRegistry();

    {
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        sprite.scale = { 5, 5 };
        sprite.position = { 0, 0, 0 };
        AssignSpriteTexture(sprite, "souls_like_knight_character:IDLE:idle1");
    }

    /*{
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        sprite.scale = { 1, 1 };
        sprite.position = { 0.5f, 0, 0 };
        AssignSpriteTexture(sprite, "souls_like_knight_character:ATTACK:attack1");
    }*/

    //{
    //    auto entity = reg.create();

    //    auto& sprite = reg.emplace<Sprite>(entity);
    //    sprite.scale = { 1, 1 };
    //    sprite.position = { 2, 0, 0 };
    //    AssignSpriteTexture(sprite, "souls_like_knight_character:IDLE:idle1");
    //}
}
