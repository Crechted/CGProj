#pragma once
#include "../../Components/SceneComponent.h"
class Box;
class TriangleComponent;

class BoxComponent : public SceneComponent
{
public:
    BoxComponent();
    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;
    void DestroyResource() override;

    Vector3 size = Vector3(0.2f);
    Vector3 color = Vector3(1.0f);

protected:
    Box* box;
};
