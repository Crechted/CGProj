#include "Box.h"

#include "../../Components/SceneComponent.h"
#include "../../Components/TriangleComponent.h"

Box::Box()
{
    triangleComp = CreateComponent<TriangleComponent>();
}

void Box::Initialize()
{
    triangleComp->topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    int32_t pointNum = 16;
    auto points = new DirectX::XMFLOAT4[pointNum]
    {
        DirectX::XMFLOAT4(+size.x, +size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, +size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, -size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, -size.y, +size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, +size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, +size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(-size.x, -size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(+size.x, -size.y, -size.z, 1.0f), DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
    };
    triangleComp->SetPoints(points, pointNum);
    
    int32_t idxCount = 16;    
    auto indexes = new int32_t[idxCount]{2, 1, 3, 0, 7, 4, 6, 5, 3, 7, 2, 6, 1, 5, 0, 4};
    triangleComp->SetIndexes(indexes, idxCount);
    
    triangleComp->Initialize();
    Object::Initialize();
}