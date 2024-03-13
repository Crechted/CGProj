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
    TriangleComponent* grid;

    float cellSize = 1.0f;
    int32_t numLineOnSide = 21;

private:
    void CreateCoordinates();
    void CreateGrid();
};
