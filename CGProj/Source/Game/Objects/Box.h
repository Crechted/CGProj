#pragma once
#include "../../Core/Object.h"

class BoxComponent;
class TriangleComponent;

class Box : public Object
{
public:
    Box();
    void Initialize() override;

    Vector3 size = Vector3(0.2f);
    Vector3 color = Vector3(1.0f);

protected:
    TriangleComponent* triangleComp;
};
