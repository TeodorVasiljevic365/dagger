#include "mage.h"
#include "golem.h"
#include "range_of_attack.h"
#include "towers.h"
#include "hp_system.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"

#include "gameplay/common/simple_collisions.h"

#include <random>

using namespace dagger;
using namespace ancient_defenders;

// Initialize static members
UInt32 ancient_defenders::WalkingPath::numberOfPoints = 0;
Sequence<Vector2> ancient_defenders::WalkingPath::path = {};

void ancient_defenders::MageBehaviorSystem::SpinUp()
{
    Engine::Dispatcher().sink<NextFrame>().connect<&MageBehaviorSystem::OnEndOfFrame>(this);
}

void ancient_defenders::MageBehaviorSystem::WindDown()
{
    Engine::Dispatcher().sink<NextFrame>().disconnect<&MageBehaviorSystem::OnEndOfFrame>(this);
}

void ancient_defenders::MageBehaviorSystem::Run()
{
    Engine::Registry().view<MageStats, Sprite, Transform, Animator, RangeOfAttack>().each(
        [](MageStats& mage_, Sprite& sprite_, Transform& transform_, Animator& animation_, RangeOfAttack& range_)
    {
        if (range_.targetFound) mage_.currentAction = EAction::Attacking;

        if (mage_.currentAction == EAction::Idling) {
            AnimatorPlay(animation_, "ancient_defenders:mage:IDLE");
        } 
        else if (mage_.currentAction == EAction::Moving) {
            auto nextPosition = mage_.postition + 1;

            auto coords = WalkingPath::path[nextPosition];

            auto destinationX = coords.x + mage_.offset.x;
            auto destinationY = coords.y + mage_.offset.y;

            if (transform_.position.x < destinationX) {
                mage_.direction.x = 1;
            }
            else if (transform_.position.x > destinationX) {
                mage_.direction.x = -1;
            }
            else mage_.direction.x = 0;
            if (transform_.position.y < destinationY) {
                mage_.direction.y = 1;
            }
            else if (transform_.position.y > destinationY) {
                mage_.direction.y = -1;
            }
            else mage_.direction.y = 0;

            if (mage_.direction.x != 0) {
                sprite_.scale.x = (-mage_.direction.x) * abs(sprite_.scale.x);
                AnimatorPlay(animation_, "ancient_defenders:mage:WALK_SIDE");
            }
            else if (mage_.direction.y == 1) {
                AnimatorPlay(animation_, "ancient_defenders:mage:WALK_UP");
            }
            else {
                AnimatorPlay(animation_, "ancient_defenders:mage:WALK_DOWN");
            }

            transform_.position.x += mage_.direction.x * mage_.speed * Engine::DeltaTime();
            transform_.position.y += mage_.direction.y * mage_.speed * Engine::DeltaTime();

            if ((transform_.position.x < destinationX && mage_.direction.x == -1) ||
                (transform_.position.x > destinationX && mage_.direction.x == 1)) {
                transform_.position.x = destinationX;
            }
            else if ((transform_.position.y > destinationY && mage_.direction.y == 1) ||
                (transform_.position.y < destinationY && mage_.direction.y == -1)) 
            {
                transform_.position.y = destinationY;
            }

            if (transform_.position.x == destinationX && transform_.position.y == destinationY) {
                mage_.postition++;
                if ((mage_.postition + 1) >= WalkingPath::numberOfPoints) {
                    mage_.currentAction = EAction::Idling;
                    return;
                }
            }

        }
        else if (mage_.currentAction == EAction::Attacking) {
            if (mage_.direction.x != 0) {
                sprite_.scale.x = (-mage_.direction.x) * abs(sprite_.scale.x);
                AnimatorPlay(animation_, "ancient_defenders:mage:ATTACK_SIDE");
            }
            else if (mage_.direction.y == 1) {
                AnimatorPlay(animation_, "ancient_defenders:mage:ATTACK_BACK");
            }
            else {
                AnimatorPlay(animation_, "ancient_defenders:mage:ATTACK_FRONT");
            }
            Engine::Registry().get<Health>(range_.target).currentHealth -= mage_.meleeDmg * Engine::DeltaTime();
            mage_.currentAction = EAction::Moving; // Go back to moving after attacking
        }
        else if (mage_.currentAction == EAction::Chanting) {
            AnimatorPlay(animation_, "ancient_defenders:mage:CHANT");
            auto & view = Engine::Registry().view<TowerStats>();

            auto & it = view.begin();
            while (it != view.end()) {
                auto& tower = view.get<TowerStats>(*it);

                if (tower.address == TowerPlacementInfo::selectedSpot && !tower.constructed) {
                    tower.constructionProgress += Engine::DeltaTime();
                }
                else if (tower.constructed) {
                    sprite_.color = { 0,0,0,0 }; // Make Mage appear to be inside of the tower by making him invisible
                    Engine::Registry().get<Sprite>(Engine::Registry().get<Health>(entt::to_entity(Engine::Registry(), mage_)).hpBar).color = { 0,0,0,0 };
                    // Make the mages HP bar also dissapear

                    mage_.currentAction = EAction::Idling;


                    TowerPlacementInfo::spotLocked = false;
                    
                }
                it++;
            }
        }
        else if (mage_.currentAction == EAction::Defending) {
            // Cast spells
        }
    });
  
}

void ancient_defenders::MageBehaviorSystem::OnEndOfFrame()
{
}

Entity ancient_defenders::Mage::Create(Vector2 position_,EAction action_, Bool offset_)
{
    auto& reg = Engine::Registry();
    auto entity = reg.create();
    
    auto& sprite = reg.emplace<Sprite>(entity);
    auto& coordinates = reg.emplace<Transform>(entity);
    auto& anim = reg.emplace<Animator>(entity);
    auto& mage = reg.emplace<MageStats>(entity);
    auto& hitbox = reg.emplace<SimpleCollision>(entity);
    auto& range = reg.emplace<RangeOfAttack>(entity);
    auto& health = reg.emplace<Health>(entity);

    health.hpBar = reg.create();
    reg.emplace<HealthBar>(health.hpBar).parent = entity;
    auto& hpBar = reg.emplace<Sprite>(health.hpBar);

    auto& mageRef = reg.emplace<Mage>(entity);
    mageRef.healthBar = health.hpBar;

    AssignSprite(sprite, "spritesheets:mage:mage_stand_side:1");
    float ratio = sprite.size.y / sprite.size.x;

    sprite.scale = { 2, 2 };

    mage.meleeDmg = 1.0f;

    mage.currentAction = action_;

    mage.speed = 50.0f;
    mage.direction = { 0,1 };

    // Randomly create offset 
    std::random_device dev;
    std::mt19937 rng(dev());

    std::uniform_int_distribution<std::mt19937::result_type> roll22(0, 22);
    std::uniform_int_distribution<std::mt19937::result_type> roll38(0, 38);

    std::uniform_int_distribution<std::mt19937::result_type> randomDirection(0, 1);
 
    // 22 is an important value because it represents the border at which character can move while still being on the path
    // 22 = 38 - 16( half of the width/height of the path - half of the character widht/height ); edge of the character sprite is alligned with the edge of the path
    // Only exception to this is when offset.y is positive at which point character can go up much higher while still appearing to walk along the path
    mage.offset = { (randomDirection(rng)?roll22(rng): 0.0f-roll22(rng)),(randomDirection(rng) ? roll38(rng) : 0.0f-roll22(rng)) };

    auto start = position_;
    if (offset_) {
        start.x += mage.offset.x;
        start.y += mage.offset.y;
    }
    // Z axis is calculated this way to make bottom most character appear closest to the screen
    coordinates.position = { start.x, start.y, std::abs(mage.offset.y + 22.0f) };

    hitbox.size = sprite.size;
    range.range = hitbox.size.x;

    range.unitType = ETarget::Mage;
    range.targetType = ETarget::Golem;

    health.currentHealth = Health::standardHP;
    health.maxHealth = Health::standardHP;

    Logger::info("Created mage");
    return entity;
}
