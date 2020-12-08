#include "player_controller.h"
#include "shoot.h"
#include "tilemap_entities.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "gameplay/common/simple_collisions.h"



using namespace dagger;
using namespace lab;

void lab::GenerateRoom(int idNext_,lab::NextLvl& currentLvl_, Transform &tr_)
{
    if(currentLvl_.id==0)
    {
        auto& view = Engine::Res<Tilemap>()["tilemaps/lab/lab.map"]->tiles;
        Engine::Registry().destroy(view.begin(),view.end());
    }
    else if(currentLvl_.id==1)
    {
        auto& view = Engine::Res<Tilemap>()["tilemaps/lab/hallway.map"]->tiles;
        Engine::Registry().destroy(view.begin(),view.end());
    }
        
  
    tr_.position = { -(tr_.position.x- tr_.position.x/3), tr_.position.y , tr_.position.z };
    TilemapLegend legend=currentLvl_.legend;       
    if(idNext_==0)
    {
        Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "tilemaps/lab/lab.map", &legend });   
    }
    else if(idNext_==1)
    {
         Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ "tilemaps/lab/hallway.map", &legend});
    }  

    currentLvl_.id=idNext_;

}


void PlayerControllerSystem::OnInitialize(Registry& registry_, Entity entity_)
{
    
    InputReceiver& receiver = registry_.get<InputReceiver>(entity_);
    for (auto command : { "rightleft", "updown", "shoot", "heavy", "light", "use","block"})
    {
        receiver.values[command] = 0;
    }
    
}

void PlayerControllerSystem::SpinUp()
{
    Engine::Registry().on_construct<InputReceiver>().connect<&PlayerControllerSystem::OnInitialize>(this);
}

void PlayerControllerSystem::Run()
{
    Engine::Registry().view<InputReceiver, Sprite, Animator, PlayerCharacter, Transform>().each(
    [](const InputReceiver input_, Sprite& sprite_, Animator& animator_, PlayerCharacter& char_, Transform& transform_)
    {
        
        Float32 rl = input_.values.at("rightleft");
        Float32 ud = input_.values.at("updown");
        Float32 shoot = input_.values.at("shoot");
        
        if(rl || ud)
        {
            AnimatorPlay(animator_, "main_character:run");
            if (rl != 0)
            { 
                sprite_.scale.x = rl;
                transform_.position.x += char_.speed * sprite_.scale.x * Engine::DeltaTime();
                    
            } 
            if (ud != 0)
            { 
                sprite_.scale.y = 1;
                transform_.position.y += char_.speed * ud * Engine::DeltaTime();
                    
            } 
            }
                
           else
            {
                AnimatorPlay(animator_, "main_character:idle");
            }
            
            if (shoot)
            {
                if (char_.cooldown <= 0)
                {
                    auto cursorInWindow = dagger::Input::CursorPositionInWorld();
                    Vector2 directions = { 1, 1 };
                    if (cursorInWindow.x < sprite_.position.x)
                        directions.x = -1;
                    else if (cursorInWindow.x == sprite_.position.x)
                        directions.x = 0;

                    if ((-1 * cursorInWindow.y) < sprite_.position.y)
                        directions.y = -1;
                    else if ((-1 * cursorInWindow.y) == sprite_.position.y)
                        directions.y = 0;

                    Float32 distanceX = cursorInWindow.x - sprite_.position.x;
                    Float32 distanceY = (cursorInWindow.y * -1) - sprite_.position.y;
                    Float32 ratio = distanceY / distanceX;
                    ratio *= (ratio > 0) ? 1 : -1;

                    Vector2 position = { sprite_.position.x, sprite_.position.y };
                    CreateBullet(position, ratio, directions);
                    char_.cooldown = 100;
                }
            }
            char_.cooldown--;


    });

    auto viewCollisions = Engine::Registry().view<Transform, SimpleCollision>();
    auto view = Engine::Registry().view<Transform,SimpleCollision,PlayerCharacter>();
    for (auto entity : view)
    {
        auto &t = view.get<Transform>(entity);
        auto &player = view.get<PlayerCharacter>(entity);
        auto &col = view.get<SimpleCollision>(entity);

        if (col.colided)
        {
            if (Engine::Registry().valid(col.colidedWith))
            {
                SimpleCollision& collision = viewCollisions.get<SimpleCollision>(col.colidedWith);
                Transform& transform = viewCollisions.get<Transform>(col.colidedWith);

                Vector2 collisionSides = col.GetCollisionSides(t.position, collision, transform.position);

                do
                {
                    Float32 dt = Engine::DeltaTime();
                    if (collisionSides.x > 0)
                    {
                        t.position.x -= (player.speed * dt);
                    }

                    if (collisionSides.y > 0)
                    {
                        t.position.y -= (player.speed* dt);
                    }
                    if (collisionSides.x < 0)
                    {
                        t.position.x += (player.speed * dt);
                    }

                    if (collisionSides.y < 0)
                    {
                        t.position.y += (player.speed* dt);
                    }
                        
                } while (col.IsCollided(t.position, collision, transform.position));
            
            
                if (Engine::Registry().has<lab::NextLvl>(col.colidedWith))
                {
                    lab::NextLvl& nextLvl = Engine::Registry().get<lab::NextLvl>(col.colidedWith);
                    lab::NextLvl& currentLvl = Engine::Registry().get<lab::NextLvl>(entity);

                    GenerateRoom(nextLvl.id,currentLvl,t);
                }
            }

            col.colided = false;
        }
    }   
 
}

void PlayerControllerSystem::WindDown()
{
    Engine::Registry().on_construct<InputReceiver>().disconnect<&PlayerControllerSystem::OnInitialize>(this);
}
