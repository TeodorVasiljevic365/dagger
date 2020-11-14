#include "pingpong_ball.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"

#include "gameplay/common/simple_collisions.h"

#include <algorithm>
#include <execution>

using namespace dagger;
using namespace ping_pong;

void PingPongBallSystem::SpinUp()
{
    Engine::Dispatcher().sink<NextFrame>().connect<&PingPongBallSystem::OnEndOfFrame>(this);
}
void PingPongBallSystem::WindDown()
{
    Engine::Dispatcher().sink<NextFrame>().disconnect<&PingPongBallSystem::OnEndOfFrame>(this);
}

void PingPongBallSystem::Run()
{
    auto viewCollisions = Engine::Registry().view<Transform, SimpleCollision>();
    auto view = Engine::Registry().view<PingPongBall, Transform, SimpleCollision>();
    for(auto entity: view) 
    {
        auto &t = view.get<Transform>(entity);
        auto &ball = view.get<PingPongBall>(entity);
        auto &col = view.get<SimpleCollision>(entity);

        if (ball.reachedGoal)
        {
            continue;
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
                    // get back for 1 frame 
                    Float32 dt = Engine::DeltaTime();
                    if (std::abs(collisionSides.x) > 0)
                    {
                        t.position.x -= (ball.speed.x * ball.speedMultiplier * dt);
                    }

                    if (std::abs(collisionSides.y) > 0)
                    {
                        t.position.y -= (ball.speed.y * ball.speedMultiplier * dt);
                    }
                        
                } while (col.IsCollided(t.position, collision, transform.position));

                if (std::abs(collisionSides.x) > 0)
                {
                    ball.speed.x *= -1;
                }

                if (std::abs(collisionSides.y) > 0)
                {
                    ball.speed.y *= -1;
                }  
            }

            if (ball.isMalicious == true &&  Engine::Registry().has<ControllerMapping>(col.colidedWith)  ) {
               auto &cntrl =  Engine::Registry().get<ControllerMapping>(col.colidedWith);
               cntrl.inverted = -1;
               ball.toBeDestroyed = true;
               break;
            }

            if (Engine::Registry().has<PingPongWall>(col.colidedWith))
            {
                PingPongWall& wall = Engine::Registry().get<PingPongWall>(col.colidedWith);
                ball.reachedGoal = true;
                ball.playerOneScored = !wall.isLeft;
            }

            col.colided = false;
        }
        else
        {
            t.position += (ball.speed * ball.speedMultiplier * Engine::DeltaTime());
        }
    }
}

void PingPongBallSystem::OnEndOfFrame()
{
   auto view = Engine::Registry().view<PingPongBall>();

   for (auto entity : view) {
       
       auto& ball = Engine::Registry().get<PingPongBall>(entity);

       if (ball.toBeDestroyed == true) {
           Engine::Registry().destroy(entity);
       }
   }
}
