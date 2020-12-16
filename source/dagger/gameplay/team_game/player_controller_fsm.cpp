#include "player_controller_fsm.h"
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
#include "core/game/transforms.h"
#include "core/graphics/text.h"
//#include <stdlib.h>


using namespace dagger;
using namespace lab;

void lab::GenerateRoom(int idNext_,lab::NextLvl& currentLvl_, Transform &tr_)
{

    auto& view = Engine::Res<Tilemap>()[currentLvl_.room[currentLvl_.id]]->tiles;
    Engine::Registry().destroy(view.begin(),view.end());
    auto& view2 = Engine::Res<Tilemap>()[currentLvl_.addons[currentLvl_.id]]->tiles;
    Engine::Registry().destroy(view2.begin(),view2.end());
    
    if(std::abs(tr_.position.x)>std::abs(tr_.position.y))
    {
    tr_.position = { -(tr_.position.x- tr_.position.x/3), tr_.position.y , tr_.position.z };
    }
    else
    {
    tr_.position = { tr_.position.x, -(tr_.position.y- tr_.position.y/2) , tr_.position.z };
    }

    TilemapLegend legend=currentLvl_.legend;       

    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ currentLvl_.room[idNext_], &legend }); 
    Engine::Dispatcher().trigger<TilemapLoadRequest>(TilemapLoadRequest{ currentLvl_.addons[idNext_], &legend });   
    

    currentLvl_.id=idNext_;

}




void PlayerControllerSystem::Run()
{
    Engine::Registry().view<CharacterFSM::StateComponent>()
        .each([&](CharacterFSM::StateComponent& state_)
            {
                characterFSM.Run(state_);
            });
            
            auto viewCollisions = Engine::Registry().view<Transform, SimpleCollision>();
            auto view = Engine::Registry().view<Transform,SimpleCollision,PlayerCharacter,Text>();
            auto viewText = Engine::Registry().view<PlayerCharacter,Text>();
            auto viewDamage = Engine::Registry().view<Bullet, Transform, SimpleCollision,PlayerCharacter>();
            for (auto entity : view)
            {
                auto &t = view.get<Transform>(entity);
                auto &player = view.get<PlayerCharacter>(entity);
                auto &col = view.get<SimpleCollision>(entity);
                auto &tex = viewText.get<Text>(entity);

                if (player.health <= 0)
                {
                    tex.alignment={ TextAlignment::CENTER };
                    tex.Set("pixel-font", "GAME OVER",{30,30});
                    Engine::Registry().destroy(entity);              
                                   
                }



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
            
            
                       

                        if (Engine::Registry().has<Bullet>(col.colidedWith))
                        {
                            Bullet &bullet = Engine::Registry().get<Bullet>(col.colidedWith);
					        player.health -= bullet.damage;
                            if(player.health<0){
                                player.health=0;
                            } 
                            tex.Set("pixel-font", std::to_string(player.health)+"/100",{10,-95,0},{10,10});
                            
                        }
                        
                        
                    

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
