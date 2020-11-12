#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace ping_pong
{
    struct ControllerMapping
    {
        EDaggerKeyboard up_key;
        EDaggerKeyboard down_key;
        EDaggerKeyboard slow_down_key;

        Vector2 input{ 0, 0 };
    };

    struct PlayerPowerUps
    {
        int  slow_down;

        bool power_up;
        bool power_down;
    };

    class PingPongPlayerInputSystem
        : public System
    {
        static Float32 s_BoarderUp;
        static Float32 s_BoarderDown;

    public:

        static Float32 s_PlayerSpeed;

        static bool s_PowerUpsActive;
        static bool s_ActivatePowerUps;
        static bool s_DeactivatePowerUps;

    public:
        inline String SystemName() { return "PingPong Player Input System"; }

        void SpinUp() override;
        void WindDown() override;
        void Run() override;

        static void SetupPlayerOneInput(ControllerMapping& controllerMapping_)
        {
            controllerMapping_.up_key = EDaggerKeyboard::KeyW;
            controllerMapping_.down_key = EDaggerKeyboard::KeyS;
            controllerMapping_.slow_down_key = EDaggerKeyboard::KeyE;
        }

        static void SetupPlayerTwoInput(ControllerMapping& controllerMapping_)
        {
            controllerMapping_.up_key = EDaggerKeyboard::KeyUp;
            controllerMapping_.down_key = EDaggerKeyboard::KeyDown;
            controllerMapping_.slow_down_key = EDaggerKeyboard::KeyL;
        }

        static void SetupPlayerBoarders(Float32 boarderUp_, Float32 boarderDown_)
        {
            s_BoarderUp = boarderUp_;
            s_BoarderDown = boarderDown_;
        }

    private:

        void OnKeyboardEvent(KeyboardEvent kEvent_);
    };
}