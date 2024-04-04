#include "MeshComponent.h"
#include "TriangleComponent.h"

MeshComponent::MeshComponent()
{
    mesh = CreateComponent<TriangleComponent>();
}

void MeshComponent::Initialize()
{
    SceneComponent::Initialize();
}
