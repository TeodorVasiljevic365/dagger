#pragma once
#include "core/system.h"
#include "core/engine.h"
#include "core/core.h"


#define STAMINA_FOR_RUNNING_FRAME 0.2f

using namespace dagger;

namespace plight {
    struct CombatStats
    {
        Float32 maxHealth{ 100 };
        Float32 currentHealth{ 100 };

        Float32 maxStamina{ 100 };
        Float32 currentStamina{ 100 };

        Entity backgroundHealthBar;
        Entity currentHealthBar;
        Float32 healthBarOffset{0.f};

        Entity backgroundStaminaBar;
        Entity currentStaminaBar;
        Float32 staminaBarOffset{0.f};

        Float32 playerDistance{ 40.f };
        
        Float32 updateTimer{ 0.01f };
        Float32 currentTimer{ 0.f };
    };

    class PlightCombatSystem : public System
    {
    public:
        inline String SystemName() { return "Plight Combat System"; }

        void Run() override;
    };
}