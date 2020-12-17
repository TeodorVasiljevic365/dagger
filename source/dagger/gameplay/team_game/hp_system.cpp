#include "hp_system.h"

#include "core/engine.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"


using namespace dagger;
using namespace ancient_defenders;

void ancient_defenders::HealthManagementSystem::SpinUp()
{
    Engine::Dispatcher().sink<NextFrame>().connect<&HealthManagementSystem::OnEndOfFrame>(this);
}

void ancient_defenders::HealthManagementSystem::WindDown()
{
    Engine::Dispatcher().sink<NextFrame>().disconnect<&HealthManagementSystem::OnEndOfFrame>(this);
}

void ancient_defenders::HealthManagementSystem::Run() {

    auto& reg = Engine::Registry();
    auto entities = reg.view<Health>();

    entities.each([&](Entity entity_, Health& health_)
        {
            auto& healthBarSprite = reg.get_or_emplace<Sprite>(health_.hpBar);
            AssignSprite(healthBarSprite, "spritesheets:hp-bar:hp_100");
            
            auto val = health_.currentHealth / health_.maxHealth;

            if (EPSILON_ZERO(val)) {
                healthBarSprite.color = { 0,0,0,0 }; // Make the previous sprite invisible; solves previous sprite staying still after character drops to low HP
            }
            else
            {
                const auto& parentSprite = reg.get<Sprite>(entity_);

                healthBarSprite.size.x = val * parentSprite.size.x;
                healthBarSprite.position = { parentSprite.position.x - ((1 - val) * (parentSprite.size.x / 2.0f)),
                    parentSprite.position.y - parentSprite.size.y / 2.0f,
                    99.0f };
            }
        });
}


void ancient_defenders::HealthManagementSystem::OnEndOfFrame()
{
    auto view = Engine::Registry().view<Health>();
    
    Sequence<Entity> toRemove{};
    auto it = view.begin();
    while (it != view.end()) {
        auto & en = view.get<Health>(*it);
        if (en.currentHealth <= 0.0f) {
            toRemove.push_back(en.hpBar);
            toRemove.push_back(*it);
        }
        it++;
    }

    Engine::Registry().destroy(toRemove.begin(), toRemove.end());
}
