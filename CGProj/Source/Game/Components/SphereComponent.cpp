#include "SphereComponent.h"

#include "Core/Components/CollisionComponent.h"
#include "Core/Components/Collisions/SphereCollision.h"
#include "Game/Objects/Sphere.h"

SphereComponent::SphereComponent(const wchar_t* pathTex, Vector3 position, float radius, int32_t sliceCount, int32_t stackCount,
    bool drawFirstHalf, bool drawSecondHalf, bool drawUp, bool drawDown)
{
    if (mesh) mesh->Destroy();
    mesh = new Sphere(pathTex, Vector3::Zero, radius, sliceCount, stackCount, drawFirstHalf, drawSecondHalf, drawUp, drawDown);
    mesh->outer = this;
    mesh->SetCollision(mesh->CreateComponent<SphereCollision>(radius));    
    //mesh->SetCollision(new SphereCollision(radius));    
    mesh->GetSceneComponent()->AttachTo(this);
    //mesh->GetCollision()->initPosition = position;
    initPosition = position;
}
