#include "Wall.h"

#include "../Components/Movement2DComponent.h"

Wall::Wall(WallType wallType)
    : type(wallType)
{
    if (type == WallType::Up)
    {
        center = Vector3(0.0f, 0.95f, 0.5f);
        extends = Vector3(0.97f, 0.02f, 0.5f);
    }

    if (type == WallType::Down)
    {
        center = Vector3(0.0f, -0.95f, 0.5f);
        extends = Vector3(0.97f, 0.02f, 0.5f);
    }
    if (type == WallType::Left)
    {
        center = Vector3(-0.95f, 0.0f, 0.5f);
        extends = Vector3(0.02f, 0.97f, 0.5f);
    }
    if (type == WallType::Right)
    {
        center = Vector3(0.95f, 0.0f, 0.5f);
        extends = Vector3(0.02f, 0.97f, 0.5f);
    }
}

Wall::Wall(WallType wallType, Vector3 center, Vector3 extends, Vector3 color)
    : type(wallType)
{
    this->center = center;
    this->extends = extends;
    this->color = color;
}

void Wall::Initialize()
{
    GameSquare::Initialize();
    moveComp->speed = 0;
}
