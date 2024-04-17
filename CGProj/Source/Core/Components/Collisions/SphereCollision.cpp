#include "SphereCollision.h"

#include "BoxCollision.h"
#include "Core/CoreTypes.h"
#include "Core/Components/DrawComponent.h"
#include "Game/Objects/Sphere.h"

SphereCollision::SphereCollision(float radius) : radius(radius)
{
}

bool SphereCollision::Contains(CollisionComponent* other) const
{
    if (const auto box = dynamic_cast<BoxCollision*>(other)) return sphereCollision.Contains(box->boxCollision);
    if (const auto sphere = dynamic_cast<SphereCollision*>(other)) return sphereCollision.Contains(sphere->sphereCollision);
    return false;
}

void SphereCollision::InitCollision()
{   
    sphereCollision.Radius = radius;
    sphereCollision.Center = DirectX::XMFLOAT3(GetLocation());
    
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Sphere::CreateDrawSphereByTopology(initPosition, radius, Vector4(0.5f, 0.0f, 1.0f, 1.0f), vertices, indexes, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    debugCollision->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    debugCollision->SetVertices(vertices);
    debugCollision->SetIndexes(indexes);
}

void SphereCollision::UpdateCollision()
{
    const DirectX::FXMMATRIX mat = GetWorldMatrix();
    sphereCollision.Transform(sphereCollision, mat);
    sphereCollision.Center = GetWorldLocation();
    sphereCollision.Radius = radius;
}
