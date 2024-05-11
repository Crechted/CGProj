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
    if (const auto mesh = dynamic_cast<Mesh*>(Owner))
    {
        Vector3 min = Vector3::Zero;
        Vector3 max = Vector3::Zero;
        mesh->GetAABB(min, max);
        if (min != Vector3::Zero || max != Vector3::Zero)
        {
            size = max - min;
            printf("size: %f, %f, %f;\n", size.x, size.y, size.z);
        }
    }
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Box::CreateDrawBoxByTopology(Vector4::Zero, size, Vector4(0.5f, 0.0f, 1.0f, 1.0f), vertices, indexes,
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    debugCollision->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    debugCollision->SetVertices(vertices);
    debugCollision->SetIndexes(indexes);

    UpdateCollision();
}

void BoxCollision::UpdateCollision()
{
    const auto trans = GetWorldTransform();
    boxCollision.Center = trans.location;
    boxCollision.Extents = size * 0.5f * trans.scale;
}
