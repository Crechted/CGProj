#include "PlayerMoveComponent.h"

#include "Core/Engine.h"
#include "Core/Components/SceneComponent.h"



bool PlayerMovement::SearchSceneComponent()
{
    if (Owner)
    {
        for (const auto comp : Owner->gameComponents)
        {
            if (const auto sceneC = dynamic_cast<SceneComponent*>(comp))
            {
                movableComp = sceneC;
                return true;
            }
        }
    }
    return false;
}

void PlayerMovement::Initialize()
{
    if (!movableComp && !SearchSceneComponent()) return;
    if (!engInst || !movableComp) return;
    Reload();
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &PlayerMovement::OnKeyDown);
    engInst->GetInputDevice()->KeyUpDelegate.AddRaw(this, &PlayerMovement::OnKeyUp);
    engInst->GetInputDevice()->MouseMoveDelegate.AddRaw(this, &PlayerMovement::OnMouseMove);
}

void PlayerMovement::Reload()
{
    if (!movableComp) return;
    movableComp->Reload();
}

void PlayerMovement::Update(float timeTick)
{
    if (!movableComp) return;
    HandleInputByKey();
    CalcOffset(timeTick);
}

void PlayerMovement::HandleInputByKey()
{
    if (!movableComp) return;
    auto curScene = movableComp->GetParentComponent()&& !movableComp->attachOnlyTranslation  ? movableComp->GetParentComponent() : movableComp;

    delLocation = Vector3(0.0);
    if (engInst->GetInputDevice()->IsKeyDown(inputForward)) delLocation += curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputRight)) delLocation += curScene->GetRight() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputBackward)) delLocation += -curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputLeft)) delLocation += -curScene->GetRight() * speed;
}

void PlayerMovement::CalcOffset(float timeTick)
{
    if (!movableComp) return;
    auto curScene = movableComp->GetParentComponent() ? movableComp->GetParentComponent() : movableComp;
    auto curRotScene = movableComp->GetParentComponent() && !movableComp->attachOnlyTranslation ? movableComp->GetParentComponent() : movableComp;

    if (curRotScene->GetRotation().x >= 89 && delRotation.x > 0 || curRotScene->GetRotation().x <= -89 && delRotation.x < 0)
    {
        delRotation.x = 0;
    }

    curRotScene->AddRotation(delRotation * timeTick);
    curScene->AddLocation(delLocation * timeTick);
    delRotation = Vector3(0.0);

    /*printf(" Rotation: Pitch=%04.4f Yaw:%04.4f Roll:%04.4f\n",
        movableComp->GetRotation().x,
        movableComp->GetRotation().y,
        movableComp->GetRotation().z);

    printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f\n",
        movableComp->GetLocation().x,
        movableComp->GetLocation().y,
        movableComp->GetLocation().z);*/
}

void PlayerMovement::OnKeyDown(Keys key)
{
}

void PlayerMovement::OnKeyUp(Keys key)
{
    if (key == Keys::R) Reload();
}

void PlayerMovement::OnMouseMove(const MouseMoveEventArgs& mouseEvent)
{
    if (!movableComp) return;

    if (engInst->GetInputDevice()->IsKeyDown(Keys::LeftShift)) return;
    delRotation.x = /*(1.0f-fmod(movableComp->Forward().z, 1.0f)) * */-mouseEvent.Offset.y * sensitive;
    //delRotation.z = fmod(movableComp->Forward().z, 1.0f)*mouseEvent.Offset.y * sensitive;
    delRotation.y = -mouseEvent.Offset.x * sensitive;
    //movableComp->rotation += delRotation;
}
