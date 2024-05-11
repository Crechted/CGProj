#include "PointLightComponent.h"

#include "Core/Components/DrawComponent.h"
#include "Game/Objects/Sphere.h"

PointLightComponent::PointLightComponent(const Transform& transform, float range, Vector4 color,  bool drawDebug)
    : range(range), color(color), drawDebug(drawDebug)
{
    sphereComp = CreateComponent<DrawComponent>();
    sceneComponent->initPosition = transform.location;
    sceneComponent->initRotation = transform.rotate;
    sceneComponent->initScale = transform.scale;
}

void PointLightComponent::Initialize()
{
    Array<VertexNoTex> vertexes;
    Array<int32_t> indexes;
    Sphere::CreateDrawSphereByTopology(Vector3::Zero, range, color, vertexes, indexes, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    sphereComp->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    sphereComp->SetVertices(vertexes);
    sphereComp->SetIndexes(indexes);
    
    LightComponent::Initialize();

    Vector3 tar = sceneComponent->GetWorldMatrix().Translation() + sceneComponent->GetForward();
    eyeData.mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), tar, sceneComponent->GetUp());
    eyeData.mProj = Matrix::CreatePerspectiveFieldOfView(RadiansFromDegree(90), 1.0f, 0.01, range);
    lightData.type = 0;
    lightData.enabled = true;
    lightData.mViewProj = eyeData.GetViewProj();
    lightData.range = range;
    lightData.color = color;
}

void PointLightComponent::Draw()
{
    if (drawDebug)
        LightComponent::Draw();
}
