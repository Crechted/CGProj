#include "SpringArmComponent.h"

#include "../Camera.h"
#include "../../Core/Game.h"
#include "../../Core/Input/InputDevice.h"
#include "../../Utils/Types.h"

SpringArmComponent::SpringArmComponent()
{
}

void SpringArmComponent::Initialize()
{
    SceneComponent::Initialize();
    game->GetInputDevice()->KeyDownDelegate.AddRaw(this, &SpringArmComponent::OnKeyDown);
    game->GetInputDevice()->KeyUpDelegate.AddRaw(this, &SpringArmComponent::OnKeyUp);
}

void SpringArmComponent::Detach()
{
    auto worldMat = controlCamera->sceneComp->GetWorldMatrix();
    auto worldLoc = worldMat.Translation();
    parentComponent = nullptr;
    SetLocation(worldLoc);
    
    auto euler = worldMat.ToEuler();
    auto pitch = DegreeFromRadians(euler.x);
    auto yaw = DegreeFromRadians(euler.y);
    auto roll = initRotation.z;
    SetRotation(Vector3(pitch, yaw, roll));
    
    springLenght = 0.0f;
}

void SpringArmComponent::Reload()
{
    SceneComponent::Reload();
    Detach();    
}

void SpringArmComponent::Update(float timeTick)
{
    SceneComponent::Update(timeTick);

    if (!controlCamera) return;
    if (springLenght <= 0.0f && delSpringLenght < 0.0f) springLenght = 0.0f;
    springLenght += delSpringLenght * timeTick;
    controlCamera->sceneComp->SetLocation(Vector3(0.0f, 0.0f, springLenght));
    
    //SetRotation(Vector3(transform.rotate.x, transform.rotate.y, 0.0f));
}

void SpringArmComponent::OnKeyDown(Keys key)
{
    if (!controlCamera) return;
    if (key == Keys::F) Detach();
    if (key == Keys::Up) delSpringLenght += -controlCamera->speedScale;
    if (key == Keys::Down) delSpringLenght -= -controlCamera->speedScale;
}

void SpringArmComponent::OnKeyUp(Keys key)
{
    if (!controlCamera) return;
    if (key == Keys::Up) delSpringLenght += controlCamera->speedScale;
    if (key == Keys::Down) delSpringLenght -= controlCamera->speedScale;
}
