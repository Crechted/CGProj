#include "BoxCollision.h"
#include "SphereCollision.h"
#include "Core/Components/DrawComponent.h"

BoxCollision::BoxCollision(Vector3 size)
    : size(size)
{
}

bool BoxCollision::Contains(CollisionComponent* other) const
{
    if (const auto box = dynamic_cast<BoxCollision*>(other)) return boxCollision.Contains(box->boxCollision);
    if (const auto sphere = dynamic_cast<SphereCollision*>(other)) return boxCollision.Contains(sphere->sphereCollision);
    return false;
}

void BoxCollision::InitCollision()
{
    boxCollision.Extents = DirectX::XMFLOAT3(size / 2);
    boxCollision.Center = DirectX::XMFLOAT3(GetLocation());

    Vector3 inPos = initPosition;
    if (const auto mesh = dynamic_cast<Mesh*>(Owner))
    {
        Vector3 min;
        Vector3 max;
        mesh->GetAABB(min, max);
        inPos = inPos != (max-min)/2 ? (max-min)/2 : inPos;
    }
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Box::CreateDrawBoxByTopology(initPosition, size, Vector4(0.5f, 0.0f, 1.0f, 1.0f), vertices, indexes, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    debugCollision->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    debugCollision->SetVertices(vertices);
    debugCollision->SetIndexes(indexes);
}

void BoxCollision::UpdateCollision()
{
    const DirectX::FXMMATRIX mat = GetWorldMatrix();
    boxCollision.Transform(boxCollision, mat);
    boxCollision.Center = GetWorldLocation();
    boxCollision.Extents = size/2;
}
