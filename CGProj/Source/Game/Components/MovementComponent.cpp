#include "MovementComponent.h"

#include "Core/Engine.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Objects/Object.h"


bool MovementComponent::SearchSceneComponent()
{
    if (Owner)
    {
        for (const auto comp : Owner->gameComponents)
        {
            if (const auto sceneC = dynamic_cast<SceneComponent*>(comp))
            {
                sceneComp = sceneC;
                return true;
            }
        }
    }
    return false;
}

void MovementComponent::Initialize()
{
    if (!sceneComp && !SearchSceneComponent()) return;
    if (!engInst || !sceneComp) return;
    Reload();
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &MovementComponent::OnKeyDown);
    engInst->GetInputDevice()->KeyUpDelegate.AddRaw(this, &MovementComponent::OnKeyUp);
    engInst->GetInputDevice()->MouseMoveDelegate.AddRaw(this, &MovementComponent::OnMouseMove);
}

void MovementComponent::Reload()
{
    if (!sceneComp) return;
    sceneComp->Reload();
}

void MovementComponent::Update(float timeTick)
{
    if (!sceneComp) return;
    HandleInputByKey();
    CalcOffset(timeTick);
}

void MovementComponent::HandleInputByKey()
{
    if (!sceneComp) return;
    auto curScene = sceneComp->GetParentComponent() && sceneComp->attachRotate ? sceneComp->GetParentComponent() : sceneComp;

    delLocation = Vector3(0.0);
    if (engInst->GetInputDevice()->IsKeyDown(inputForward)) delLocation += curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputRight)) delLocation += curScene->GetRight() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputUp)) delLocation += curScene->GetGlobalUp() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputBackward)) delLocation += -curScene->GetForward() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputLeft)) delLocation += -curScene->GetRight() * speed;
    if (engInst->GetInputDevice()->IsKeyDown(inputDown)) delLocation += -curScene->GetGlobalUp() * speed;
}

void MovementComponent::CalcOffset(float timeTick)
{
    if (!sceneComp) return;
    auto curScene = sceneComp->GetParentComponent() ? sceneComp->GetParentComponent() : sceneComp;
    auto curRotScene = sceneComp->GetParentComponent() && sceneComp->attachRotate ? sceneComp->GetParentComponent() : sceneComp;

    if (curRotScene->GetRotation().x >= 89 && delRotation.x > 0 || curRotScene->GetRotation().x <= -89 && delRotation.x < 0)
    {
        delRotation.x = 0;
    }

    curRotScene->AddRotation(delRotation * timeTick);
    curScene->AddLocation(delLocation * timeTick);
    delRotation = Vector3(0.0);

    /*printf(" Rotation: Pitch=%04.4f Yaw:%04.4f Roll:%04.4f\n",
        sceneComp->GetRotation().x,
        sceneComp->GetRotation().y,
        sceneComp->GetRotation().z);

    printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f\n",
        sceneComp->GetLocation().x,
        sceneComp->GetLocation().y,
        sceneComp->GetLocation().z);*/
}

void MovementComponent::OnKeyDown(Keys key)
{
}

void MovementComponent::OnKeyUp(Keys key)
{
    if (key == Keys::R) Reload();
}

void MovementComponent::OnMouseMove(const MouseMoveEventArgs& mouseEvent)
{
    if (!sceneComp) return;

    if (engInst->GetInputDevice()->IsKeyDown(Keys::LeftShift)) return;
    delRotation.x = /*(1.0f-fmod(sceneComp->Forward().z, 1.0f)) * */-mouseEvent.Offset.y * sensitive;
    //delRotation.z = fmod(sceneComp->Forward().z, 1.0f)*mouseEvent.Offset.y * sensitive;
    delRotation.y = -mouseEvent.Offset.x * sensitive;
    //sceneComp->rotation += delRotation;
}
