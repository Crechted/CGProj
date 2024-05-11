#pragma once
#include "LightComponent.h"
#include "Core/Components/SceneComponent.h"

struct Transform;
class DrawComponent;

class SpotLightComponent : public LightComponent
{
public:
    SpotLightComponent(const Transform& transform = Transform(), float range = 2.0f, float angle = 15.0f, Vector4 color = Vector4::One, bool drawDebug = false);
    void Initialize() override;
    void Draw() override;
    void SetDebugVisible(bool visible) {drawDebug = visible;}

protected:
    DrawComponent* coneComp;
    float range;
    float angle;
    Vector4 color;
    bool drawDebug;
};
