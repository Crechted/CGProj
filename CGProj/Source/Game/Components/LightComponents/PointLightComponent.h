#pragma once
#include "LightComponent.h"
#include "Core/Components/SceneComponent.h"

struct Transform;
class DrawComponent;

class PointLightComponent : public LightComponent
{
public:
    PointLightComponent(const Transform& transform = Transform(), float range = 2.0f, Vector4 color = Vector4::One, bool drawDebug = false);
    void Initialize() override;
    void Draw() override;
    void SetDebugVisible(bool visible) {drawDebug = visible;}

protected:
    DrawComponent* sphereComp;
    float range;
    Vector4 color;
    bool drawDebug;
};
