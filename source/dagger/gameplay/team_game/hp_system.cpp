#include "hp_system.h"

#include "core/engine.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"


using namespace dagger;
using namespace ancient_defenders;

Sequence<Float32> ancient_defenders::Health::hpSteps = {0, 11, 22, 33, 44, 55, 66, 77, 88, 100 };

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
    auto entities = reg.view<HealthBar, Sprite>();

    entities.each([&](Entity healthBarEntity, HealthBar& healthBar, Sprite& healthBarSprite)
        {
            auto parentEntity = healthBar.parent;
            auto health = reg.get<Health>(parentEntity);
            auto val = closestNeighbour(100.0f * health.currentHealth / health.maxHealth);

            if (EPSILON_ZERO(val)) {
                healthBarSprite.color = { 0,0,0,0 }; // Make the previous sprite invisible; solves previous sprite staying still after character drops to low HP
//                return; // Since there is no sprite for 0 hp, skip adding it
            }
            else
            {
                AssignSprite(healthBarSprite, "spritesheets:hp-bar:hp_" + std::to_string((UInt32)val));
                const auto& parentSprite = reg.get<Sprite>(parentEntity);

                auto ratio = healthBarSprite.size.x / healthBarSprite.size.y;
                healthBarSprite.size = { parentSprite.size.x, parentSprite.size.x / ratio };
                healthBarSprite.position = { parentSprite.position.x,
                    parentSprite.position.y - parentSprite.size.y / 2.0f - parentSprite.size.y / 2.0f,
                    parentSprite.position.z };
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

Float32 ancient_defenders::closestNeighbour(Float32 number_)
{
    SInt32 numOfSteps = Health::hpSteps.size()-1;
    
    SInt32 i = 0;
    while (number_ > Health::hpSteps[i]) i++;

    if (i == 0) {
        return Health::hpSteps[0];
    }
    else {
        if (std::abs(Health::hpSteps[i - 1] - number_) < std::abs(Health::hpSteps[i] - number_)) {
            return Health::hpSteps[i - 1];
        }
        else {
            return Health::hpSteps[i];
        }
    }
}
