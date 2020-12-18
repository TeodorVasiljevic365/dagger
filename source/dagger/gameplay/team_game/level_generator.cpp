#include "level_generator.h"

#include "core/game/transforms.h"
#include "core/graphics/sprite.h"

using namespace dagger;

Sequence<Entity> level_generator::smiljana::CreateFloor(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    sprite.position = { x_ * 16 - 125.0f, y_ * 16 - 50.0f, 30 };
    AssignSprite(sprite, "spritesheets:among_them_tilemap:floor_1");


    return { entity };
}

Sequence<Entity> level_generator::smiljana::CreateWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = reg_.create();
    auto& sprite = reg_.emplace<Sprite>(entity);
    sprite.position = { x_ * 16 - 125.0f, y_ * 16 - 50.0f, 30 };
    AssignSprite(sprite, "spritesheets:among_them_tilemap:wall_1");

    return { entity };
}

Entity level_generator::jovica::CreateTile(Registry& reg_, SInt32 x_, SInt32 y_, SInt32 z_, String name_)
{
    Entity entity = reg_.create();

    auto& transform = reg_.emplace<Transform>(entity);
    transform.position = { x_ * 16, y_ * 16, z_ };

    auto& sprite = reg_.emplace<Sprite>(entity);
    AssignSprite(sprite, name_);

    return entity;
}

Sequence<Entity> level_generator::jovica::CreateFloor(Registry& reg_, SInt32 x_, SInt32 y_)
{
    UInt32 type = 1 + rand() % 10;
    Entity entity = CreateTile(reg_, x_, y_, 30, fmt::format("spritesheets:among_them_tilemap:floor_{}", type));

    return { entity };
}

Sequence<Entity> level_generator::jovica::Nothing(Registry& reg_, SInt32 x_, SInt32 y_)
{
    return {};
}

Sequence<Entity> level_generator::jovica::CreateTopWall(Registry& reg_, SInt32 x_, SInt32 y_)
{

    UInt32 type = 1 + rand() % 6;
    Entity entity = CreateTile(reg_, x_, y_, 20, fmt::format("spritesheets:among_them_tilemap:wall_{}", type));
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top");

    return { entity, top };
}

Sequence<Entity> level_generator::jovica::CreateBottomWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_bottom");

    return { entity };
}

Sequence<Entity> level_generator::jovica::CreateLeftWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_left");

    return { entity };
}

Sequence<Entity> level_generator::jovica::CreateRightWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_right");

    return { entity };
}

Sequence<Entity> level_generator::jovica::CreateTopLeftWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_left");
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_left");
    Entity peak = CreateTile(reg_, x_, y_ + 2, 0, "spritesheets:among_them_tilemap:wall_top_inner_left");

    return { entity, top, peak };
}

Sequence<Entity> level_generator::jovica::CreateTopRightWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_right");

    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_right");

    Entity peak = CreateTile(reg_, x_, y_ + 2, 0, "spritesheets:among_them_tilemap:wall_top_inner_right");

    return { entity, top, peak };
}

Sequence<Entity> level_generator::jovica::CreateBottomLeftWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_bottom_inner_left");

    return { entity };
}

Sequence<Entity> level_generator::jovica::CreateBottomRightWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity entity = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_bottom_inner_right");

    return { entity };
}

Sequence<Entity> level_generator::jovica::CreateBottomLeftConcWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity right = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_left");
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_bottom_right");

    return { right, top };
}

Sequence<Entity> level_generator::jovica::CreateBottomRightConcWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    Entity left = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_top_right");
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_bottom_left");

    return { left, top };
}

Sequence<Entity> level_generator::jovica::CreateTopLeftConcWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    UInt32 type = 1 + rand() % 6;
    Entity entity = CreateTile(reg_, x_, y_, 20, fmt::format("spritesheets:among_them_tilemap:wall_{}", type));
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_inner_left_2");

    return { entity, top };
}

Sequence<Entity> level_generator::jovica::CreateTopRightConcWall(Registry& reg_, SInt32 x_, SInt32 y_)
{
    UInt32 type = 1 + rand() % 6;
    Entity entity = CreateTile(reg_, x_, y_, 20, fmt::format("spritesheets:among_them_tilemap:wall_{}", type));
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_inner_right_2");

    return { entity, top };
}

Sequence<Entity> level_generator::jovica::CreateBottomLeftConcWallS(Registry & reg_, SInt32 x_, SInt32 y_)
{
    Entity right = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_bottom_inner_left");
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_bottom_right");

    return { right, top };
}

Sequence<Entity> level_generator::jovica::CreateBottomRightConcWallS(Registry & reg_, SInt32 x_, SInt32 y_)
{
    Entity left = CreateTile(reg_, x_, y_, 0, "spritesheets:among_them_tilemap:wall_bottom_inner_right");
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_bottom_left");

    return { left, top };
}

Sequence<Entity> level_generator::jovica::CreateTopLeftConcWallS(Registry & reg_, SInt32 x_, SInt32 y_)
{
    UInt32 type = 1 + rand() % 6;
    Entity entity = CreateTile(reg_, x_, y_, 20, fmt::format("spritesheets:among_them_tilemap:wall_{}", type));
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_inner_left_2");
    Entity peak = CreateTile(reg_, x_, y_ + 2, 0, "spritesheets:among_them_tilemap:wall_top_inner_left");

    return { entity, top, peak };
}

Sequence<Entity> level_generator::jovica::CreateTopRightConcWallS(Registry & reg_, SInt32 x_, SInt32 y_)
{
    UInt32 type = 1 + rand() % 6;
    Entity entity = CreateTile(reg_, x_, y_, 20, fmt::format("spritesheets:among_them_tilemap:wall_{}", type));
    Entity top = CreateTile(reg_, x_, y_ + 1, 0, "spritesheets:among_them_tilemap:wall_top_inner_right_2");
    Entity peak = CreateTile(reg_, x_, y_ + 2, 0, "spritesheets:among_them_tilemap:wall_top_inner_right");

    return { entity, top, peak };
}
