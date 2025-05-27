#include "PlayerMoveComponent.h"

#include "Core/Engine.h"
#include "Core/Components/SceneComponent.h"
#include "Game/Components/SpringArmComponent.h"
#include "Game/Catomary/Objects/CatamariPawn.h"
#include "Game/Components/SphereComponent.h"

void PlayerMoveComponent::Initialize()
{
    pawn = dynamic_cast<CatamariPawn*>(Owner);
    if (!pawn) return;
    engInst->GetInputDevice()->MouseMoveDelegate.AddRaw(this, &PlayerMoveComponent::OnMouseMove);
    GameComponent::Initialize();
}

void PlayerMoveComponent::Reload()
{
    if (!pawn) return;
}

void PlayerMoveComponent::Update(float timeTick)
{
    if (!pawn) return;
    HandleInputByKey();
    CalcOffset(timeTick);
}

void PlayerMoveComponent::HandleInputByKey()
{
    if (!pawn) return;
    auto curScene = pawn->GetSpringArmComponent();

    delLocation = Vector3(0.0);
    if (engInst->GetInputDevice()->IsKeyDown(inputForward)) delLocation += curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputRight)) delLocation += curScene->GetRight() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputBackward)) delLocation += -curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputLeft)) delLocation += -curScene->GetRight() * speed;
    delLocation.y = 0.0f;
    delLocation.Normalize();
}

void PlayerMoveComponent::CalcOffset(float timeTick)
{
    if (!pawn) return;
    auto curScene = pawn->GetSphereComponent();
    auto curRotScene = pawn->GetSpringArmComponent();

    auto curRot = curRotScene->GetRotation();
    if (curRot.x >= 89 && delRotation.x > 0 || curRot.x <= -89 && delRotation.x < 0)
    {
        delRotation.x = 0;
        curRot.x = curRot.x >= 89.0f ? 89.0f : -89.0f;
        curRotScene->SetRotation(curRot);
    }

    const auto mat = curRotScene->GetWorldMatrix();
    curRotScene->AddRotation(delRotation * timeTick);
    curRot = curRotScene->GetRotation();
    curScene->AddQuatRotation(Vector3(delLocation.z, 0.0f, -delLocation.x) * speed * rotSpeed * timeTick);
    curScene->AddLocation(delLocation * speed * timeTick);

    if (curRot.x >= 89 && delRotation.x > 0 || curRot.x <= -89 && delRotation.x < 0)
    {
        delRotation.x = 0;
        curRot.x = curRot.x >= 89.0f ? 89.0f : -89.0f;
        curRotScene->SetRotation(curRot);
    }
    delRotation = Vector3(0.0);

    /*printf(" Rotation: Pitch=%04.4f Yaw:%04.4f Roll:%04.4f\n",
        curRotScene->GetRotation().x,
        curRotScene->GetRotation().y,
        curRotScene->GetRotation().z);

    printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f\n",
        curScene->GetLocation().x,
        curScene->GetLocation().y,
        curScene->GetLocation().z);*/
}

void PlayerMoveComponent::OnMouseMove(const MouseMoveEventArgs& mouseEvent)
{
    if (!pawn) return;

    if (engInst->GetInputDevice()->IsKeyDown(Keys::LeftShift)) return;
    delRotation.x = /*(1.0f-fmod(movableComp->Forward().z, 1.0f)) * */-mouseEvent.Offset.y * sensitive;
    //delRotation.z = fmod(movableComp->Forward().z, 1.0f)*mouseEvent.Offset.y * sensitive;
    delRotation.y = -mouseEvent.Offset.x * sensitive;
    //movableComp->rotation += delRotation;
}
