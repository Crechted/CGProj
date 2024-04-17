#include "CatomaryObjects.h"

#include "Core/Components/CollisionComponent.h"

CatomaryObjects::CatomaryObjects(const std::string& pathModel, const wchar_t* pathTex)
{
    initPathModel = pathModel;
    initPathTex = pathTex;
}

void CatomaryObjects::Initialize()
{
    Mesh::Initialize();
    collisionComp->beginOverlapped.AddRaw(this, &CatomaryObjects::OnBeginOverlap);
}

void CatomaryObjects::OnBeginOverlap(CollisionComponent* other)
{
    GetSceneComponent()->AttachTo(other);
    //GetSceneComponent()->SetRotation(GetSceneComponent()->GetWorldRotation());
    //GetSceneComponent()->SetLocation(GetSceneComponent()->GetWorldLocation());
}
