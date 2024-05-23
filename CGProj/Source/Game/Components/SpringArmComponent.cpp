#include "SpringArmComponent.h"

#include "Game/Camera.h"
#include "Core/Engine.h"
#include "Core/Input/InputDevice.h"
#include "Utils/HelperFunctions.h"

SpringArmComponent::SpringArmComponent(Camera* cam)
{
    SetControlCamera(cam);
}

void SpringArmComponent::Initialize()
{
    if (!controlCamera)
    {
        printf("Forget Set Camera In Spring Component");
    }

    SceneComponent::Initialize();
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &SpringArmComponent::OnKeyDown);
    engInst->GetInputDevice()->KeyUpDelegate.AddRaw(this, &SpringArmComponent::OnKeyUp);
}

void SpringArmComponent::Detach()
{
    if (!controlCamera) return;

    controlCamera->SetTargetView(nullptr);
    auto worldMat = controlCamera->GetSceneComponent()->GetWorldMatrix();
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
    if (controlCamera)
    {
        controlCamera->Reload();
    }
}

void SpringArmComponent::SetControlCamera(Camera* cam)
{
    if (!cam) return;
    controlCamera = cam;
    controlCamera->GetSceneComponent()->AttachTo(this);
    controlCamera->SetTargetView(this);
}

void SpringArmComponent::Update(float timeTick)
{
    SceneComponent::Update(timeTick);
    if (!controlCamera) return;
    controlCamera->Update(timeTick);
    if (springLenght <= 0.0f && delSpringLenght < 0.0f) springLenght = 0.0f;
    springLenght += delSpringLenght * timeTick;
    controlCamera->GetSceneComponent()->SetLocation(Vector3(0.0f, 0.0f, springLenght));

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
