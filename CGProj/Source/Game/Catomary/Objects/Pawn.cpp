#include "Pawn.h"
#include "Core/Engine.h"
#include "Game/Catomary/Components/PlayerMoveComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Components/SpringArmComponent.h"

Pawn::Pawn()
{
    springArmComp = CreateComponent<SpringArmComponent>(engInst->CreateCamera());
    sphereComp = CreateComponent<SphereComponent>(nullptr, Vector3(0.0f, 0.5f, 0.0f));
    playerMoveComp = CreateComponent<PlayerMoveComponent>();
    springArmComp->AttachTo(sphereComp);
    springArmComp->attachOnlyTranslation = true;
    springArmComp->springLenght = 5.0f;
}

void Pawn::Initialize()
{
    engInst->SetCurCamera(springArmComp->GetCamera());

    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Pawn::OnKeyDown);
    Object::Initialize();
}

void Pawn::Update(float timeTick)
{
    Object::Update(timeTick);
}

void Pawn::OnKeyDown(Keys key)
{
    if (key == Keys::R) Reload();
}
