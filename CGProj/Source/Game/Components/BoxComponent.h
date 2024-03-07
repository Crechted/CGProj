#pragma once
#include "../../Components/SceneComponent.h"

class TriangleComponent;

class BoxComponent : public SceneComponent
{
public:
    BoxComponent();
    void Initialize() override;
    TriangleComponent* triangleComp;

    Vector3 center = Vector3(0.0f);
    Vector3 size = Vector3(0.2f);
    Vector3 color = Vector3(1.0f);
};
