#include "SpotLightComponent.h"

#include "Utils/HelperFunctions.h"
#include "Core/Components/DrawComponent.h"
#include "Utils/DebugDrawing.h"

SpotLightComponent::SpotLightComponent(const Transform& transform, float range, float angle, Vector4 color, bool drawDebug)
    : range(range), angle(angle), color(color), drawDebug(drawDebug)
{
    coneComp = CreateComponent<DrawComponent>();
    sceneComponent->initPosition = transform.location;
    sceneComponent->initRotation = transform.rotate;
    sceneComponent->initScale = transform.scale;
}

void SpotLightComponent::Initialize()
{

    Array<VertexNoTex> vertexes;
    Array<int32_t> indexes;
    DebugDrawing::CreateDrawCone(Vector3::Zero, angle, range, 16, color,
        sceneComponent->GetTransform().GetMatrix(), vertexes, indexes);
    coneComp->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    coneComp->SetVertices(vertexes);
    coneComp->SetIndexes(indexes);

     DebugDrawing::CreateDrawCone(Vector3::Zero, angle, range, 16, color, sceneComponent->GetTransform().GetMatrix(), volumeVertices, volumeIndexes, true);

    LightComponent::Initialize();
    Vector3 tar = sceneComponent->GetWorldMatrix().Translation() + sceneComponent->GetForward();
    eyeData.mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), tar, sceneComponent->GetUp());
    eyeData.mProj = Matrix::CreatePerspectiveFieldOfView(RadiansFromDegree(angle), 1.0f, 0.01f, range);
    lightData.type = LightType::SpotLight;
    lightData.enabled = true;
    lightData.mViewProj = eyeData.GetViewProj();
    lightData.range = range;
    lightData.spotlightAngle = angle;
    lightData.color = color;
}

void SpotLightComponent::Update(float timeTick)
{
    sceneComponent->AddQuatRotation(Vector3(0.0f, timeTick*50.0f, 0.0f));
    LightComponent::Update(timeTick);
}

void SpotLightComponent::Draw()
{
    if (drawDebug)
        LightComponent::Draw();
}
