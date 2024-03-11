#pragma once
#include "../../Core/Object.h"

class SceneComponent;
class TriangleComponent;

class Grids : public Object
{
public:

    void Initialize() override;
    Grids();
    SceneComponent* sceneComp;
    TriangleComponent* coordAxis;
    TriangleComponent* grids;

    float cellSize = 1.0f;
    int32_t numLineOnSide = 40;
};
