#pragma once
#include "Core/Components/SceneComponent.h"

class DrawBox;


class DrawBoxComponent : public SceneComponent
{
public:
    DrawBoxComponent(Vector3 position = Vector3::Zero,
        Vector3 size = Vector3(0.2f),
        Vector4 color = Vector4(1.0f),
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    void Update(float timeTick) override;
    void Initialize() override;
    void Draw() override;
    void DestroyResource() override;

    DrawBox* GetDrawBox() const { return box; }

protected:
    DrawBox* box;
};
