#include "BoxComponent.h"

#include "../../Components/TriangleComponent.h"
#include "../../Core/Object.h"

BoxComponent::BoxComponent()
{
}

void BoxComponent::Initialize()
{
    if (Owner)
    {
        triangleComp = Owner->CreateComponent<TriangleComponent>();
    }
    else return;

    triangleComp->topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    int32_t pointNum = 16;

    triangleComp->SetPoints(new DirectX::XMFLOAT4[pointNum]
    {
        DirectX::XMFLOAT4(+size.x, +size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, +size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, -size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, -size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, +size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, +size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, -size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, -size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
    }, pointNum);
    int32_t idxCount = 16;
    triangleComp->SetIndexes(new int32_t[idxCount]{2, 1, 3, 0, 7, 4, 6, 5, 3, 7, 2, 6, 1, 5, 0, 4}, idxCount);
    triangleComp->Initialize();
    SceneComponent::Initialize();
}
