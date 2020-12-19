#include "plight_game_logic.h"

#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"
#include "core/graphics/text.h"

#include "gameplay/plight/plight.h"
#include "gameplay/plight/plight_controller.h"
#include "gameplay/plight/plight_combat.h"
#include "gameplay/plight/plight_aiming.h"
#include "gameplay/plight/plight_collisions.h"
#include "gameplay/plight/plight_projectiles.h"

using namespace dagger;
using namespace plight;

void PlightGameLogicSystem::SpinUp()
{
    Engine::Dispatcher().sink<NextFrame>().connect<&PlightGameLogicSystem::OnEndOfFrame>(this);
}

void PlightGameLogicSystem::WindDown()
{
    Engine::Dispatcher().sink<NextFrame>().disconnect<&PlightGameLogicSystem::OnEndOfFrame>(this);
}

void PlightGameLogicSystem::Run()
{

        auto view1 = Engine::Registry().view<PlightGameInfo>();
        for (auto entity : view1) {
            auto& gameInfo = view1.get<PlightGameInfo>(entity);
            if (gameInfo.newGame) {
                auto& text = Engine::Registry().get<Text>(gameInfo.newGameMessage);
                text.Set("pixel-font", gameInfo.newGameMessageString);
                gameInfo.newGame = false;
                gameInfo.displayingMessage = true;
            }
            else if (gameInfo.displayingMessage) {
                gameInfo.currentMessageDuration += Engine::DeltaTime();
                if (gameInfo.currentMessageDuration >= gameInfo.newGameMessageDuration) {
                    gameInfo.displayingMessage = false;
                    gameInfo.currentMessageDuration = 0.f;
                    auto& text = Engine::Registry().get<Text>(gameInfo.newGameMessage);
                    for (auto ent : text.entities) {
                        Engine::Registry().remove_all(ent);
                    }
                }
            }
            
        }
    
        auto view = Engine::Registry().view<InputReceiver>();
        for (auto entity : view)
        {
            auto& input = view.get<InputReceiver>(entity);
            if (EPSILON_NOT_ZERO(input.Get("restart"))) {
                m_Restart = true;
            }
        }
    
}

void PlightGameLogicSystem::OnEndOfFrame()
{
    if (m_Restart)
    {
        m_Restart = false;

        plight::ResetCharacters();


       auto view2 = Engine::Registry().view<Projectile>();
       for (auto entity : view2) {
           Engine::Registry().remove_all(entity);
       }
      
    }
}