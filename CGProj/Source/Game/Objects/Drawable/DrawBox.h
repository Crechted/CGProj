#pragma once
#include <d3dcommon.h>

#include "SimpleMath.h"
#include "Core/Objects/Object.h"

class SceneComponent;
class DrawComponent;
using namespace DirectX::SimpleMath;

class DrawBox : public Object
{
public:
    DrawBox(Vector3 position = Vector3::Zero,
        Vector3 size = Vector3(0.2f),
        Vector4 color = Vector4(1.0f),
        D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    void Initialize() override;
    DrawComponent* GetDrawComponent() const { return drawComp; }
    Vector3 initSize;
    Vector4 initColor;
    void Draw() override;

protected:
    D3D_PRIMITIVE_TOPOLOGY topology;
    SceneComponent* sceneComp = nullptr;
    DrawComponent* drawComp = nullptr;
};
