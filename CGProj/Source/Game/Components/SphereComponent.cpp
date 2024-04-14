#include "SphereComponent.h"
#include "Game/Objects/Sphere.h"

SphereComponent::SphereComponent(const wchar_t* pathTex, Vector3 position, float radius, int32_t sliceCount, int32_t stackCount,
    bool drawFirstHalf, bool drawSecondHalf, bool drawUp, bool drawDown)
{
    if (mesh) mesh->Destroy();
    mesh = new Sphere(pathTex, position, radius, sliceCount, stackCount, drawFirstHalf, drawSecondHalf, drawUp, drawDown);
}
