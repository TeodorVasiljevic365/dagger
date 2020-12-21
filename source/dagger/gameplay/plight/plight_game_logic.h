#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace plight
{
    struct PlightGameInfo {
        bool newGame = true;
        bool displayingMessage = false;
        bool displayingMessage2 = false;
        String newGameMessageString1 = "New Game!";
        String newGameMessageString2 = "Battle!";

        Entity newGameMessage;

        Float32 newGameMessageDuration = 2.f;
        Float32 currentMessageDuration = 0.f;
    };
   
    class PlightGameLogicSystem
        : public System
    {
        bool m_Restart = false;
       

    public:
        inline String SystemName() { return "Plight Game Logic System System"; }

        void SpinUp() override;
        void WindDown() override;
        void Run() override;

    private:
        void OnEndOfFrame();

    };
}
