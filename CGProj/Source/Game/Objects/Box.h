#pragma once
#include "SimpleMath.h"
#include "../../Core/Object.h"

using namespace DirectX::SimpleMath;
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
