#include "CatamariPawn.h"

#include "CatamariObjects.h"
#include "Core/Engine.h"
#include "Core/Components/CollisionComponent.h"
#include "Game/Catomary/Components/PlayerMoveComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Components/SpringArmComponent.h"

CatamariPawn::CatamariPawn(float lenght, Vector3 pos)
{
    springArmComp = CreateComponent<SpringArmComponent>(engInst->CreateCamera());
    sphereComp = CreateComponent<SphereComponent>(nullptr, pos);
    playerMoveComp = CreateComponent<PlayerMoveComponent>();
    springArmComp->AttachTo(sphereComp);
    springArmComp->attachRotate = false;
    springArmComp->attachScale = false;
    springArmComp->springLenght = lenght;
}

void CatamariPawn::Initialize()
{
    engInst->SetCurCamera(springArmComp->GetCamera());
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &CatamariPawn::OnKeyDown);

    sphereComp->GetMeshCollision()->beginOverlapped.AddRaw(this, &CatamariPawn::OnBeginOverlap);
    sphereComp->SetScale(Vector3(2.f));
    Object::Initialize();
}

void CatamariPawn::OnBeginOverlap(CollisionComponent* other)
{
    if (!other || !other->Owner || !dynamic_cast<CatamariObjects*>(other->Owner)) return;
    /*sphereComp->SetScale(sphereComp->GetScale() * 1.05f);
    const auto WorldLoc = sphereComp->GetWorldLocation();
    sphereComp->SetWorldLocation(Vector3(WorldLoc.x, WorldLoc.y * 1.05f, WorldLoc.z));*/
    // Что-то ломается в повороте, при изменении размера, поэтому хрен с ним
}


void CatamariPawn::OnKeyDown(Keys key)
{
    if (key == Keys::R) Reload();
}