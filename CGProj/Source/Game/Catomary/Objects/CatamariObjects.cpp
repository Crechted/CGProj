#include "CatamariObjects.h"

#include "CatamariPawn.h"
#include "Core/Components/CollisionComponent.h"

CatamariObjects::CatamariObjects(const std::string& pathModel, const wchar_t* pathTex)
{
    initPathModel = pathModel;
    initPathTex = pathTex;
}

void CatamariObjects::Initialize()
{
    Mesh::Initialize();
    collisionComp->beginOverlapped.AddRaw(this, &CatamariObjects::OnBeginOverlap);
}

void CatamariObjects::OnBeginOverlap(CollisionComponent* other)
{
    if(!other || !other->Owner || !other->Owner->outer || !other->Owner->outer->Owner ||!dynamic_cast<CatamariPawn*>(other->Owner->outer->Owner)) return;
    const Transform worldTrans = GetSceneComponent()->GetWorldTransform();
    GetSceneComponent()->attachScale = true;
    GetSceneComponent()->AttachTo(other);
    GetSceneComponent()->SetWorldTransform(worldTrans);
    SetCollisionEnabled(false);
    //GetSceneComponent()->SetLocation(GetSceneComponent()->GetWorldLocation());
}
