#include "Wall.h"

#include "../../../Components/Movement2DComponent.h"
#include "../../../Components/TriangleComponent.h"

Wall::Wall(WallType wallType)
    : type(wallType)
{
    if (type == WallType::Up)
    {
        center = Vector3(0.0, 0.95, 0.5);
        extends = Vector3(0.97, 0.02, 0.5);
    }

    if (type == WallType::Down)
    {
        center = Vector3(0.0, -0.95, 0.5);
        extends = Vector3(0.97, 0.02, 0.5);
    }
    if (type == WallType::Left)
    {
        center = Vector3(-0.95, 0.0, 0.5);
        extends = Vector3(0.02, 0.97, 0.5);
    }
    if (type == WallType::Right)
    {
        center = Vector3(0.95, 0.0, 0.5);
        extends = Vector3(0.02, 0.97, 0.5);
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
