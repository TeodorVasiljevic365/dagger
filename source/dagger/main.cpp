
#include "core/optimization.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/textures.h"
#include "core/graphics/texture.h"
#include "core/graphics/window.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/animations.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/gui.h"
#include "tools/diagnostics.h"
#include "tools/console.h"
#include "tools/toolmenu.h"

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

using namespace dagger;

void CoreSystemsSetup(Engine &engine)
{
    engine.AddSystem<WindowSystem>(800, 600);
    engine.AddSystem<InputSystem>();
    engine.AddSystem<ShaderSystem>();
    engine.AddSystem<TextureSystem>();
    engine.AddSystem<SpriteRenderSystem>();
    engine.AddSystem<AnimationSystem>();
#if !defined(NDEBUG)
    engine.AddSystem<DiagnosticSystem>();
    engine.AddSystem<GUISystem>();
    engine.AddSystem<ToolMenuSystem>();
#endif //!defined(NDEBUG)
}

class PlayerInfoSystem : public System
{
    String SystemName() override {
        return "Player Info System";
    }

    inline void SpinUp() override
    {
        Engine::Registry().view<InputReceiver>().each([](InputReceiver& receiver_)
            {
                receiver_.values["Player:run"] = 0;
                receiver_.values["Player:jump"] = 0;
                receiver_.values["Player:down"] = 0;
                receiver_.values["Player:heavy"] = 0;
                receiver_.values["Player:light"] = 0;
                receiver_.values["Player:use"] = 0;
            });
    }

    inline void Run() override
    {
        Engine::Registry().view<InputReceiver>().each([](InputReceiver receiver_)
            {
                Logger::critical("Player [ run {} | jump {} | down {} | heavy {} | light {} | use {} ]",
                    receiver_.values["Player:run"],
                    receiver_.values["Player:jump"],
                    receiver_.values["Player:down"],
                    receiver_.values["Player:heavy"],
                    receiver_.values["Player:light"],
                    receiver_.values["Player:use"]);
            });
    }
};

void GameplaySystemsSetup(Engine &engine)
{
    engine.AddSystem<PlayerInfoSystem>();
}

void PostInitWorld(Engine &engine)
{
    ShaderSystem::Use("standard");

    auto& reg = engine.GetRegistry();
    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    sprite.scale = 500.f;
    auto& anim = reg.emplace<Animator>(entity);
    AnimatorPlay(anim, "souls_like_knight_character:ATTACK");
    auto& input = reg.emplace<InputReceiver>(entity);
    input.contexts.push_back("Player");
}

int main(int argc_, char** argv_)
{
	srand(time(0));
	Logger::set_level(Logger::level::trace);

	Engine engine;
    CoreSystemsSetup(engine);
    GameplaySystemsSetup(engine);

	EngineInit(engine);

    PostInitWorld(engine);

	// Game loop starts here 
	while (engine.Up())
	{
		EngineLoop(engine);
	}

	EngineStop(engine);

	return 0;
}
