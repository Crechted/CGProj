#pragma once
#include "SceneComponent.h"

class TriangleComponent;

class MeshComponent : public SceneComponent
{
public:
    MeshComponent();
    void Initialize() override;

private:
    TriangleComponent* mesh;
};
