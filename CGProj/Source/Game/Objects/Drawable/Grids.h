#pragma once
#include "Core/Objects/Object.h"

class DrawComponent;
class SceneComponent;

class Grids : public Object
{
public:

    void Initialize() override;
    Grids(float cell = 1.0f, int32_t numLineOnSide = 21);
    SceneComponent* sceneComp;
    DrawComponent* coordAxis;
    DrawComponent* grid;

    float cellSize = 1.0f;
    int32_t numLineOnSide = 21;

private:
    void CreateCoordinates();
    void CreateGrid();
};
