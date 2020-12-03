#include "team_game_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"

using namespace team_game;

void PlatformerControllerSystem::OnInitialize(Registry& registry_, Entity entity_)
{
    InputReceiver& receiver = registry_.get<InputReceiver>(entity_);
    for (auto command : { "run", "jump", "down", "roll", "heavy", "light", "use" })
    {
        receiver.values[command] = 0;
    }
}

void PlatformerControllerSystem::SpinUp()
{
    Engine::Registry().on_construct<InputReceiver>().connect<&PlatformerControllerSystem::OnInitialize>(this);
}

Float32 team_game::CalculateVerticalSpeed(Float32 speed_, Float32 acceleration_, Float32 time_)
{
    return speed_ - acceleration_ * time_;
}


// Ex: If the player is pressing the left movement key and roll Key
// allow him to do that. Alligning the sprite_.scale.x with run variable enables
// the character to start rolling in the direction it is facing
void team_game::AlignCharacter(Float32 run_, Sprite& sprite_)
{
    if (run_ != 0)
    {
        sprite_.scale.x = run_;
    }
}

void PlatformerControllerSystem::Run()
{
    Engine::Registry().view<InputReceiver, Sprite, Transform, PlatformerCharacter>().each(
        [](const InputReceiver input_, Sprite& sprite_, Transform& transform_, PlatformerCharacter& char_)
        {
            Float32 roll = input_.values.at("roll");
            Float32 run = input_.values.at("run");
            Float32 jump = input_.values.at("jump");

            if (run != 0)
            {
                if (char_.isJumping)
                {
                    char_.turningDuringJump = true;
                }
                else if (jump == 1)
                {
                    char_.runningJump = true;
                }
            }

            if (roll == 1 && !char_.isJumping)
            {
                char_.isRolling = true;
                AlignCharacter(run, sprite_);
            }

            if (jump == 1 && !char_.isRolling)
            {
                char_.isJumping = true;
                AlignCharacter(run, sprite_);
            }

            if (char_.isRolling || char_.isJumping)
            {
                if (char_.isRolling)
                {
                    AlignCharacter(run, sprite_);
                    if (char_.timeRolling < char_.rollingTime)
                    {
                        if ((sprite_.scale.x == 1) || (sprite_.scale.x == -1))
                        {
                            sprite_.position.x += char_.rollingSpeed * sprite_.scale.x * Engine::DeltaTime();
                            transform_.position.x += char_.rollingSpeed * sprite_.scale.x * Engine::DeltaTime();
                            char_.timeRolling += Engine::DeltaTime();
                        }
                        else
                        {
                            char_.isRolling = false;
                            char_.timeRolling = 0;
                        }
                    }
                    else
                    {
                        char_.isRolling = false;
                        char_.timeRolling = 0;
                    }
                }

                else if (char_.isJumping)
                {
                    AlignCharacter(run, sprite_);
                    char_.verticalSpeed = CalculateVerticalSpeed(char_.verticalInitialSpeed, char_.gravity, char_.timeJumping);
                    if (!char_.reachedMax)
                    {
                    }
                    else
                    {
                    }

                    char_.timeJumping += Engine::DeltaTime();
                    sprite_.position.y += char_.verticalSpeed * Engine::DeltaTime();
                    transform_.position.y += char_.verticalSpeed * Engine::DeltaTime();
                    if ((sprite_.scale.x == 1) || (sprite_.scale.x == -1))
                    {
                        if (char_.runningJump)
                        {
                            sprite_.position.x += char_.speed * sprite_.scale.x * Engine::DeltaTime();
                            transform_.position.x += char_.speed * sprite_.scale.x * Engine::DeltaTime();
                        }
                        else if (char_.turningDuringJump)
                        {
                            sprite_.position.x += char_.speed / 2 * sprite_.scale.x * Engine::DeltaTime();
                            transform_.position.x += char_.speed / 2 * sprite_.scale.x * Engine::DeltaTime();
                        }
                    }

                    if (char_.verticalSpeed <= 0)
                    {
                        char_.reachedMax = true;
                    }
                    if (char_.canGoDown == false)
                    {
                        char_.reachedMax = false;
                        char_.isJumping = false;
                        char_.timeJumping = 0;
                    }
                }
            }
            else
            {
                if (run == 0)
                {
                    char_.runningJump = false;
                    char_.turningDuringJump = false;
                }
                else
                {
                    sprite_.scale.x = run;
                    if ((run == 1) || (run == -1))
                    {
                        sprite_.position.x += char_.speed * sprite_.scale.x * Engine::DeltaTime();
                        transform_.position.x += char_.speed * sprite_.scale.x * Engine::DeltaTime();
                    }
                }
            }
        });
}

void PlatformerControllerSystem::WindDown()
{
    Engine::Registry().on_construct<InputReceiver>().disconnect<&PlatformerControllerSystem::OnInitialize>(this);
}