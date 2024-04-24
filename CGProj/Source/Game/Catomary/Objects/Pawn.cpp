#include "Pawn.h"
#include "Core/Engine.h"
#include "Game/Catomary/Components/PlayerMoveComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Components/SpringArmComponent.h"

Pawn::Pawn(float lenght, Vector3 pos)
{
    springArmComp = CreateComponent<SpringArmComponent>(engInst->CreateCamera());
    sphereComp = CreateComponent<SphereComponent>(nullptr, pos);
    playerMoveComp = CreateComponent<PlayerMoveComponent>();
    springArmComp->AttachTo(sphereComp);
    springArmComp->attachRotate = false;
    springArmComp->springLenght = lenght;
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
