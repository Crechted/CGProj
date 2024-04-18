#include "CatamaryObjects.h"

#include "Pawn.h"
#include "Core/Components/CollisionComponent.h"

CatamaryObjects::CatamaryObjects(const std::string& pathModel, const wchar_t* pathTex)
{
    initPathModel = pathModel;
    initPathTex = pathTex;
}

void CatamaryObjects::Initialize()
{
    Mesh::Initialize();
    collisionComp->beginOverlapped.AddRaw(this, &CatamaryObjects::OnBeginOverlap);
}

void CatamaryObjects::OnBeginOverlap(CollisionComponent* other)
{
    if(!dynamic_cast<Pawn*>(other->Owner->outer->Owner)) return;
    const Transform worldTrans = GetSceneComponent()->GetWorldTransform();
    GetSceneComponent()->attachScale = true;
    GetSceneComponent()->AttachTo(other);
    GetSceneComponent()->SetWorldTransform(worldTrans);
    SetCollisionEnabled(false);
    //GetSceneComponent()->SetLocation(GetSceneComponent()->GetWorldLocation());
}
