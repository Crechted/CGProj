#pragma once
#include "PongObject.h"
#include "../../GameSquare.h"

enum class WallType : uint8_t
{
    Up,
    Down,
    Left,
    Right
};

class Wall : public PongObject
{
public:
    Wall (WallType wallType);
    Wall(WallType wallType, Vector3 center,
        Vector3 extends,
        Vector3 color);

    void Initialize() override;

    WallType type;
};
