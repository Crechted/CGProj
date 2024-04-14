#pragma once
#include "Core/Components/SceneComponent.h"

class DrawSphere;

class DrawSphereComponent : public SceneComponent
{
public:
    DrawSphereComponent();
    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;
    void DestroyResource() override;

    float radius = 0.5f;
    int32_t sliceCount = 16;
    int32_t stackCount = 16;
    Vector3 color = Vector3(1.0f);
    bool doDifColor = false;
    bool drawFirstHalf = true;
    bool drawSecondHalf = true;
    bool drawUp = true;
    bool drawDown = true;
    
protected:
    DrawSphere* sphere;
};
