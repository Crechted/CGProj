#include "Spectator.h"

#include "Core/Engine.h"
#include "Core/Input/InputDevice.h"
#include "Game/Components/MovementComponent.h"
#include "Game/Components/SphereComponent.h"
#include "Game/Components/SpringArmComponent.h"

Spectator::Spectator(float lenght, Vector3 pos)
{
    springArmComp = CreateComponent<SpringArmComponent>(engInst->CreateCamera());
    springArmComp->initPosition = pos;
    moveComp = CreateComponent<MovementComponent>();
    springArmComp->attachRotate = false;
    springArmComp->springLenght = lenght;
}

void Spectator::Initialize()
{
    engInst->SetCurCamera(springArmComp->GetCamera());
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Spectator::OnKeyDown);
    Object::Initialize();
}


void Spectator::OnKeyDown(Keys key)
{
    if (key == Keys::R) Reload();
}
