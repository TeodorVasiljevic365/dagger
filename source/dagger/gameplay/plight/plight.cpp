#define _USE_MATH_DEFINES
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
#include "core/graphics/text.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "tools/diagnostics.h"
#include <time.h>
#include <math.h>

#include "gameplay/plight/plight_tilemaps_initialization.h"
#include "gameplay/plight/plight_controller.h"
#include "gameplay/plight/plight_combat.h"
#include "gameplay/plight/plight_collisions.h"
#include "gameplay/plight/plight_aiming.h"
#include "gameplay/plight/plight_physics.h"
#include "gameplay/plight/plight_projectiles.h"
#include "gameplay/plight/tilemaps.h"
#include "gameplay/plight/plight_game_logic.h"
#include "gameplay/plight/plight_spikes.h"
#include "gameplay/plight/plight_particles.h"



using namespace dagger;
using namespace plight;

struct PlightCharacter
{
    Entity entity;
    Sprite& sprite;
    Animator& animator;
    InputReceiver& input;
    PlightCharacterController& character;
    PlightCollision& col;
    Transform& transform;
    plight::PhysicsObject& physics;
    CombatStats& cstats;
    PlightCrosshair& crosshair;


    static PlightCharacter Get(Entity entity_)
    {
        auto& reg = Engine::Registry();
        auto& sprite = reg.get_or_emplace<Sprite>(entity_);
        auto& anim = reg.get_or_emplace<Animator>(entity_);
        auto& input = reg.get_or_emplace<InputReceiver>(entity_);

        auto& character = reg.get_or_emplace<PlightCharacterController>(entity_);
        auto& col = reg.get_or_emplace<PlightCollision>(entity_);
        auto& transform = reg.get_or_emplace<Transform>(entity_);
        auto& physics = reg.get_or_emplace<plight::PhysicsObject>(entity_);
        physics.is_static = false;
        physics.my_groups.push_back(1);
        physics.collision_groups.push_back(1);
        auto& cstats = reg.get_or_emplace<CombatStats>(entity_);
        auto& crosshair = reg.get_or_emplace<PlightCrosshair>(entity_);

        return PlightCharacter{ entity_, sprite, anim, input, character, col, transform, physics, cstats, crosshair};

    }

    static PlightCharacter Create(
        String input_ = "",
        ColorRGB color_ = { 1, 1, 1 },
        Vector2 position_ = { 0, 0 })
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto chr = PlightCharacter::Get(entity);


        ATTACH_TO_FSM(PlightCharacterControllerFSM, entity);


        chr.sprite.scale = { 1, 1 };
        chr.sprite.position = { position_, 82.f };
        chr.sprite.color = { color_, 1.0f };

        chr.col.size.x = 16;
        chr.col.size.y = 16;

        chr.transform.position = { position_, 82.f };
        chr.character.startPosition = position_;

        AssignSprite(chr.sprite, "spritesheets:dungeon:knight_m_idle_anim:1");
        AnimatorPlay(chr.animator, "Plight:knight_m:IDLE");

        if (input_ != "")
            chr.input.contexts.push_back(input_);

        chr.character.speed = 100.f;

        chr.crosshair.crosshairSprite = reg.create();
        chr.crosshair.angle = 0.f;
        chr.crosshair.playerDistance = 20.f;
        auto& crosshairSprite = reg.emplace<Sprite>(chr.crosshair.crosshairSprite);
        AssignSprite(crosshairSprite, "Plight:crosshair:crosshair1");
        crosshairSprite.position.x = chr.sprite.position.x + chr.crosshair.playerDistance;
        crosshairSprite.position.y = chr.sprite.position.y;
        crosshairSprite.position.z = chr.sprite.position.z;

        ProjectileSpawnerSettings projectile_settings;
        projectile_settings.projectileDamage = 5.f;
        projectile_settings.projectileSpeed = 175.f;
        projectile_settings.pSpriteName = "Plight:projectiles:Arrow_1";

        ProjectileSystem::SetupProjectileSystem(entity, projectile_settings);

        //Particle spawner for taking damage
        PlightParticleSpawnerSettings particle_settings;
        particle_settings.Setup(0.05f, { 6.f, 6.f }, { -0.35f, -0.30f }, { 0.35f, 0.f },
            { 1.f,0.f,0.f,1 }, { 1.f,0.f,0.f,1 }, "EmptyWhitePixel", false, .5f,0.5f);
        PlightParticleSystem::SetupParticleSystem(entity, particle_settings);

        return chr;
    }
};

void Plight::GameplaySystemsSetup(Engine &engine_)
{

    engine_.AddSystem<PlightControllerSystem>();
    engine_.AddSystem<PlightCollisionsSystem>();
    engine_.AddSystem<plight::PhysicsSystem>();
    engine_.AddSystem<PlightCombatSystem>();
    engine_.AddSystem<PlightAimingSystem>();
    engine_.AddSystem<TilemapSystem>();
    engine_.AddSystem<ProjectileSystem>();
    engine_.AddSystem<PlightGameLogicSystem>();
    engine_.AddSystem<PlightSpikesSystem>();
    engine_.AddSystem<PlightParticleSystem>();

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

    srand(time(NULL));
	plight::SetupMainManu(engine_);
    plight::SetupTilemaps();
    plight::SetupWorld_AimingSystem(engine_);

}

void plight::SetupWorld(Engine &engine_)
{

}

void plight::ResetCharacters()
{
    auto view = Engine::Registry().view<PlightCharacterController>();
    for (auto entity : view) {
        auto character = PlightCharacter::Get(entity);
        character.cstats.currentHealth = character.cstats.maxHealth;
        character.cstats.currentStamina = character.cstats.maxStamina;
        character.transform.position.x = character.character.startPosition.x;
        character.transform.position.y = character.character.startPosition.y;

        auto& currentHealthBar = Engine::Registry().get<Sprite>(character.cstats.currentHealthBar);
        auto& currentStaminaBar = Engine::Registry().get<Sprite>(character.cstats.currentStaminaBar);
        auto& backgroundHealthBar = Engine::Registry().get<Sprite>(character.cstats.backgroundHealthBar);
        auto& backgroundStaminaBar = Engine::Registry().get<Sprite>(character.cstats.backgroundStaminaBar);

        currentHealthBar.size.x = 50;
        currentStaminaBar.size.x = 50;
        currentHealthBar.position.x = backgroundHealthBar.position.x;
        currentStaminaBar.position.x = backgroundStaminaBar.position.x;
        character.cstats.healthBarOffset = 0.f;
        character.cstats.staminaBarOffset = 0.f;

        auto& crosshairSprite = Engine::Registry().get<Sprite>(character.crosshair.crosshairSprite);
        crosshairSprite.position.x = character.transform.position.x + character.crosshair.playerDistance;
        crosshairSprite.position.y = character.transform.position.y;
        crosshairSprite.position.z = character.transform.position.z;

        character.crosshair.angle = character.crosshair.startAngle;
        if (character.crosshair.angle > 0.f) {
            auto& crosshairSprite = Engine::Registry().get<Sprite>(character.crosshair.crosshairSprite);
            crosshairSprite.position.x -= character.crosshair.playerDistance * 2;
            character.sprite.scale.x = -1;
        }

        character.character.dashing = false;
        character.character.running = false;
        character.character.resting = false;
        character.character.dead = false;
        
        
    }
}

void setUpBackground(Engine& engine_) {
    auto& reg = engine_.Registry();
    for (int i = -30; i < 30; i++)
    {
        for (int j = -30; j < 30; j++)
        {
            auto entity = reg.create();
            auto& sprite = reg.emplace<Sprite>(entity);
            AssignSprite(sprite, fmt::format("spritesheets:dungeon:floor_{}", 1 + (rand() % 8)));
            sprite.position = { i * 16, j * 16, 10 };
        }
    }

}

void plight::SetupWorld_test1(Engine& engine_) {
   
    setUpBackground(engine_);

    auto mainChar = PlightCharacter::Create("ASDW_topdown", { 1, 1, 1 }, { 0, 0 });

}

void plight::SetupWorld_CombatSystem(Engine& engine_){
    setUpBackground(engine_);

    auto mainChar = PlightCharacter::Create("ASDW_topdown", { 1, 1, 1 }, { -356,32 });

    auto backgroundHealthBar1 = Engine::Registry().create();
    auto currentHealthBar1 = Engine::Registry().create();

    auto backgroundStaminaBar1 = Engine::Registry().create();
    auto currentStaminaBar1 = Engine::Registry().create();

    mainChar.cstats.backgroundHealthBar = backgroundHealthBar1;
    mainChar.cstats.currentHealthBar = currentHealthBar1;
    mainChar.cstats.backgroundStaminaBar = backgroundStaminaBar1;
    mainChar.cstats.currentStaminaBar = currentStaminaBar1;

    auto& backgroundSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.backgroundHealthBar);

    AssignSprite(backgroundSprite, "EmptyWhitePixel");
    backgroundSprite.color = { 0, 0, 0, 1 };
    backgroundSprite.size = { 50, 5 };
    backgroundSprite.scale = { 1, 1 };
    backgroundSprite.position = { 0, 0, 2 };

    auto& frontSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.currentHealthBar);

    AssignSprite(frontSprite, "EmptyWhitePixel");
    frontSprite.color = { 1, 0, 0, 1 };
    frontSprite.size = { 50, 5 };
    frontSprite.scale = { 1, 1 };
    frontSprite.position = { 0, 0, 1 };

    auto& backgroundStaminaSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.backgroundStaminaBar);

    AssignSprite(backgroundStaminaSprite, "EmptyWhitePixel");
    backgroundStaminaSprite.color = { 0, 0, 0, 1 };
    backgroundStaminaSprite.size = { 50, 5 };
    backgroundStaminaSprite.scale = { 1, 1 };
    backgroundStaminaSprite.position = { 0, 0, 2 };

    auto& frontStaminaSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.currentStaminaBar);

    AssignSprite(frontStaminaSprite, "EmptyWhitePixel");
    frontStaminaSprite.color = { 0, 1, 0, 1 };
    frontStaminaSprite.size = { 50, 5 };
    frontStaminaSprite.scale = { 1, 1 };
    frontStaminaSprite.position = { 0, 0, 1 };



    auto sndChar = PlightCharacter::Create("arrows_topdown", { 1, 0, 0 }, { 356,32 });


    auto backgroundHealthBar2 = Engine::Registry().create();
    auto currentHealthBar2 = Engine::Registry().create();
    auto backgroundStaminaBar2 = Engine::Registry().create();
    auto currentStaminaBar2 = Engine::Registry().create();

    sndChar.cstats.backgroundHealthBar = backgroundHealthBar2;
    sndChar.cstats.currentHealthBar = currentHealthBar2;
    sndChar.cstats.backgroundStaminaBar = backgroundStaminaBar2;
    sndChar.cstats.currentStaminaBar = currentStaminaBar2;

    auto& backgroundSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.backgroundHealthBar);

    AssignSprite(backgroundSprite2, "EmptyWhitePixel");
    backgroundSprite2.color = { 0, 0, 0, 1 };
    backgroundSprite2.size = { 50, 5 };
    backgroundSprite2.scale = { 1, 1 };
    backgroundSprite2.position = { 0, 0, 2 };

    auto& frontSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.currentHealthBar);

    AssignSprite(frontSprite2, "EmptyWhitePixel");
    frontSprite2.color = { 1, 0, 0, 1 };
    frontSprite2.size = { 50, 5 };
    frontSprite2.scale = { 1, 1 };
    frontSprite2.position = { 0, 0, 1 };

    auto& backgroundStaminaSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.backgroundStaminaBar);

    AssignSprite(backgroundStaminaSprite2, "EmptyWhitePixel");
    backgroundStaminaSprite2.color = { 0, 0, 0, 1 };
    backgroundStaminaSprite2.size = { 50, 5 };
    backgroundStaminaSprite2.scale = { 1, 1 };
    backgroundStaminaSprite2.position = { 0, 0, 2 };

    auto& frontStaminaSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.currentStaminaBar);

    AssignSprite(frontStaminaSprite2, "EmptyWhitePixel");
    frontStaminaSprite2.color = { 0, 1, 0, 1 };
    frontStaminaSprite2.size = { 50, 5 };
    frontStaminaSprite2.scale = { 1, 1 };
    frontStaminaSprite2.position = { 0, 0, 1 };
}


void plight::SetupMainManu(Engine& engine_)
{
	auto entity = Engine::Registry().create();

	auto& pintro = Engine::Registry().emplace<PlightIntro>(entity);
	auto& Title = Engine::Registry().emplace<Sprite>(entity);
	AssignSprite(Title, "Plight:logo:gametitle");
	Title.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Title.size = { 500, 200 };
	Title.scale = { 1, 1 };
	Title.position = { 0, 50, 0 };

	auto& input = Engine::Registry().emplace<InputReceiver>(entity);
	input.contexts.push_back("mainmenu_input");
	
}


void plight::SetupWorld_AimingSystem(Engine& engine_)
{
    auto entity = Engine::Registry().create();
    auto& pgInfo = Engine::Registry().emplace<PlightGameInfo>(entity);

    auto mainChar = PlightCharacter::Create("asdw_circular", { 1, 1, 1 }, { -356, 32 });
    mainChar.crosshair.startAngle = 0.f;
    mainChar.character.playerNumber = "Player 1";

    auto backgroundHealthBar1 = Engine::Registry().create();
    auto currentHealthBar1 = Engine::Registry().create();

    auto backgroundStaminaBar1 = Engine::Registry().create();
    auto currentStaminaBar1 = Engine::Registry().create();

    mainChar.cstats.backgroundHealthBar = backgroundHealthBar1;
    mainChar.cstats.currentHealthBar = currentHealthBar1;
    mainChar.cstats.backgroundStaminaBar = backgroundStaminaBar1;
    mainChar.cstats.currentStaminaBar = currentStaminaBar1;

    auto& backgroundSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.backgroundHealthBar);

    AssignSprite(backgroundSprite, "EmptyWhitePixel");
    backgroundSprite.color = { 0, 0, 0, 1 };
    backgroundSprite.size = { 50, 5 };
    backgroundSprite.scale = { 1, 1 };
    backgroundSprite.position = { -356, 57, 2 };


    auto& frontSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.currentHealthBar);

    AssignSprite(frontSprite, "EmptyWhitePixel");
    frontSprite.color = { 1, 0, 0, 1 };
    frontSprite.size = { 50, 5 };
    frontSprite.scale = { 1, 1 };
    frontSprite.position = { -356, 57, 0 };


    auto& backgroundStaminaSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.backgroundStaminaBar);

    AssignSprite(backgroundStaminaSprite, "EmptyWhitePixel");
    backgroundStaminaSprite.color = { 0, 0, 0, 1 };
    backgroundStaminaSprite.size = { 50, 5 };
    backgroundStaminaSprite.scale = { 1, 1 };
    backgroundStaminaSprite.position = { -356, 47, 2 };


    auto& frontStaminaSprite = Engine::Registry().emplace<Sprite>(mainChar.cstats.currentStaminaBar);

    AssignSprite(frontStaminaSprite, "EmptyWhitePixel");
    frontStaminaSprite.color = { 0, 1, 0, 1 };
    frontStaminaSprite.size = { 50, 5 };
    frontStaminaSprite.scale = { 1, 1 };
    frontStaminaSprite.position = { -356, 47, 0 };


    auto sndChar = PlightCharacter::Create("arrows_circular", { 1, 0, 0 }, { 356, 32 });
    sndChar.crosshair.angle = M_PI;
    sndChar.crosshair.startAngle = M_PI;
    auto& crosshairSprite = Engine::Registry().get<Sprite>(sndChar.crosshair.crosshairSprite);
    crosshairSprite.position.x -= sndChar.crosshair.playerDistance * 2;
    sndChar.character.playerNumber = "Player 2";

    auto backgroundHealthBar2 = Engine::Registry().create();
    auto currentHealthBar2 = Engine::Registry().create();
    auto backgroundStaminaBar2 = Engine::Registry().create();
    auto currentStaminaBar2 = Engine::Registry().create();

    sndChar.cstats.backgroundHealthBar = backgroundHealthBar2;
    sndChar.cstats.currentHealthBar = currentHealthBar2;
    sndChar.cstats.backgroundStaminaBar = backgroundStaminaBar2;
    sndChar.cstats.currentStaminaBar = currentStaminaBar2;

    auto& backgroundSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.backgroundHealthBar);

    AssignSprite(backgroundSprite2, "EmptyWhitePixel");
    backgroundSprite2.color = { 0, 0, 0, 1 };
    backgroundSprite2.size = { 50, 5 };
    backgroundSprite2.scale = { 1, 1 };
    backgroundSprite2.position = { 356, 57, 2 };


    auto& frontSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.currentHealthBar);

    AssignSprite(frontSprite2, "EmptyWhitePixel");
    frontSprite2.color = { 1, 0, 0, 1 };
    frontSprite2.size = { 50, 5 };
    frontSprite2.scale = { 1, 1 };
    frontSprite2.position = { 356, 57, 0};


    auto& backgroundStaminaSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.backgroundStaminaBar);

    AssignSprite(backgroundStaminaSprite2, "EmptyWhitePixel");
    backgroundStaminaSprite2.color = { 0, 0, 0, 1 };
    backgroundStaminaSprite2.size = { 50, 5 };
    backgroundStaminaSprite2.scale = { 1, 1 };
    backgroundStaminaSprite2.position = { 356, 47, 2 };


    auto& frontStaminaSprite2 = Engine::Registry().emplace<Sprite>(sndChar.cstats.currentStaminaBar);

    AssignSprite(frontStaminaSprite2, "EmptyWhitePixel");
    frontStaminaSprite2.color = { 0, 1, 0, 1 };
    frontStaminaSprite2.size = { 50, 5 };
    frontStaminaSprite2.scale = { 1, 1 };
    frontStaminaSprite2.position = { 356, 47, 0 };

}

void plight::SetupTilemaps()
{
    TilemapLegend floorLegend;
    floorLegend['.'] = &CreateFloor;
    floorLegend[','] = &CreateBlackBackground;
    floorLegend['S'] = &CreateFloorSpikes;
    floorLegend['R'] = &CreateWallMid;
    floorLegend['C'] = &CreateRoof;
    floorLegend['P'] = &CreateWallColumn;

    TilemapLegend wallLegend;
    wallLegend['.'] = &CreateEmpty;
    wallLegend['1'] = &CreateWallSideTopLeft;
    wallLegend['2'] = &CreateWallSideTopRight;
    wallLegend['3'] = &CreateWallCornerBottomLeft;
    wallLegend['4'] = &CreateWallCornerBottomRight;
    wallLegend['J'] = &CreateWallCornerMidRight;
    wallLegend['|'] = &CreateWallSideMidLeft;
    wallLegend['}'] = &CreateSideWallMidRight;
    wallLegend['#'] = &CreateWallMid;
    wallLegend['L'] = &CreateWallSideFrontLeft;
    wallLegend['_'] = &CreateFrontWall;
    wallLegend['-'] = &CreateWallCornerRight;
    wallLegend['R'] = &CreateWallSideFrontRight;
    wallLegend['Z'] = &CreateWallBannerBlue;
    wallLegend['X'] = &CreateWallBannerRed;
    wallLegend['K'] = &CreateWallCornerLeft;
    wallLegend['G'] = &CreateWallGoo;
    wallLegend['U'] = &CreateBlueFountain;
    wallLegend['V'] = &CreateRedFountain;


    TilemapLegend featuresLegend;
    featuresLegend[','] = &CreateEmpty;
    featuresLegend['.'] = &CreateEmpty;

    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "tilemaps/map1_floor.map", &floorLegend });
    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "tilemaps/map1_walls.map", &wallLegend });
    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "tilemaps/map1_features.map", &featuresLegend });
}



