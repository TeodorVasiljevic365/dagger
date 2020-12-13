#include "team_game_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"


#include "gameplay/team_game/brawler_controller.h"
#include "gameplay/team_game/game_manager.h"
#include "gameplay/team_game/player_camera_focus.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/team_game/physics.h"
#include "gameplay/team_game/collisions.h"

using namespace dagger;
using namespace team_game;

void TeamGame::GameplaySystemsSetup(Engine &engine_)
{
    engine_.AddPausableSystem<CameraFollowSystem>();
    engine_.AddPausableSystem<PhysicsSystem>();
    engine_.AddPausableSystem<GameManagerSystem>();
    engine_.AddPausableSystem<BrawlerControllerSystem>();
    engine_.AddPausableSystem<SimpleCollisionsSystem>();
    engine_.AddPausableSystem<CollisionSystem>();
    engine_.AddPausableSystem<TransformSystem>();
}


void SetupCamera()
{
    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 2;
    camera->position = { 0, 0, 0 };
    camera->Update();
}






struct Player
{
    Entity entity;
    Sprite& sprite;
    Animator& animator;
    InputReceiver& input;
    BrawlerCharacter& character;
    Transform& transform;
    Physics& physics;
    SimpleCollision& col;


    static Player Get(Entity entity)
    {
        auto& reg = Engine::Registry();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& anim = reg.get_or_emplace<Animator>(entity);
        auto& input = reg.get_or_emplace<InputReceiver>(entity);
        auto& character = reg.get_or_emplace<BrawlerCharacter>(entity);
        auto& transform = reg.get_or_emplace<Transform>(entity);
        auto& physics = reg.get_or_emplace<Physics>(entity);
        auto& col=reg.get_or_emplace<SimpleCollision>(entity);
        ATTACH_TO_FSM(ControllerFSM, entity);
        ATTACH_TO_FSM(AnimationsFSM, entity);
        col.size.x = 5;
        col.size.y = 15;
        physics.nonStatic = true;
        return Player{ entity, sprite, anim, input, character,transform,physics,col };
    }

    static Player Create(
        String input_ = "",
        ColorRGB color_ = { 1, 1, 1 },
        Vector2 position_ = { 0, 0 })
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto chr = Player::Get(entity);

        chr.sprite.scale = { 1, 1 };
        chr.sprite.position = { position_, 1.0f };
        chr.sprite.color = { color_, 1.0f };
        
        chr.transform.position = { position_,0.0f };

        AssignSprite(chr.sprite, "Light:idle");
        AnimatorPlay(chr.animator, "character:IDLE");


        if (input_ != "")
            chr.input.contexts.push_back(input_);

        chr.character.speed.x = 100;
        chr.character.speed.y = 250;

        return chr;
    }
};

void CreateBackground()
{
    auto& reg = Engine::Registry();
    auto* camera = Engine::GetDefaultResource<Camera>();

    /* Create terrain */ {
        auto back = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(back);
        AssignSprite(sprite, "EmptyWhitePixel");
        sprite.color = { 0, 0, 0, 1 };
        sprite.size = { 200, 200 };
        sprite.scale = { 10, 1 };
        sprite.position = { 0, -125, 1 };
        
       
    }

    // down(ground) platfrom
    {
        auto entity = reg.create();
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x =  800;
        col.size.y = 200;

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x = 0;
        transform.position.y = -125;
        transform.position.z = 1;
    }

    // left wall
    {
        auto back = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(back);
        AssignSprite(sprite, "EmptyWhitePixel");
        sprite.color = { 0, 0, 0, 1 };
        sprite.size = { 100, 500 };
        sprite.scale = { 1, 1 };
        sprite.position = { -200, -75, 1 };
        auto entity = reg.create();
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x = 100;
        col.size.y = 500;

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x =-200;
        transform.position.y = -75;
        transform.position.z = 1;
    }

    /* Put background image */ {
        auto entity = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);

        AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Background");
        sprite.position.z = 10;
    }
    
    /* Put grass */ {
        auto entity = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);

        AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Grass");
        sprite.position = { 0, -25, 5 };
    }

    /* Put trees */ {
        auto entity = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);

        AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Tree");
        sprite.position = { 0, 30, 7 };
    }
}

void team_game::SetupWorld(Engine& engine_)
{
    SetupCamera();
    CreateBackground();

    auto mainChar = Player::Create("CONTROLS", { 1, 1, 1 }, { 0, 0 });
    Engine::Registry().emplace<CameraFollow>(mainChar.entity);



}

void team_game::SetupWorld_Demo(Engine& engine_)
{
    SetupCamera();
    //CreateBackground();

    auto mainChar = Player::Create("CONTROLS", { 1, 1, 1 }, { 0, 100 });
    Engine::Registry().emplace<CameraFollow>(mainChar.entity);
}
void TeamGame::WorldSetup(Engine& engine_)
{
    SetupWorld_Demo(engine_);
}