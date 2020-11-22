#pragma once
#if defined(DAGGER_DEBUG)

#include "core/system.h"
#include "core/core.h"

#include "tools/toolmenu.h"

using namespace dagger;

namespace ping_pong
{
    class PingPongGameplay
        : public System
        , public Subscriber<ToolMenuRender>
        , public Subscriber<NextFrame>
    {
        bool vsPlayer = true;
        bool vsAI = false;

    public:
        
        inline String SystemName() { return "Ping Pong Gameplay System"; }

        void SpinUp() override;
        void Run() override;
        void WindDown() override;

    protected:
        void RenderToolMenu();
        void OnEndOfFrame();
    };
}

#endif //defined(DAGGER_DEBUG)