#include "SphereCollision.h"

#include "BoxCollision.h"
#include "Core/CoreTypes.h"
#include "Core/Components/DrawComponent.h"
#include "Game/Objects/Drawable/Sphere.h"

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
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Sphere::CreateDrawSphere(Vector3::Zero, radius, 16, 16, Vector4(0.5f, 0.0f, 1.0f, 1.0f), vertices, indexes);
    debugCollision->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    debugCollision->SetVertices(vertices);
    debugCollision->SetIndexes(indexes);
    UpdateCollision();
}

void SphereCollision::UpdateCollision()
{
    const auto trans = GetWorldTransform();
    sphereCollision.Center = trans.location;
    sphereCollision.Radius = radius*trans.scale.x;
}
