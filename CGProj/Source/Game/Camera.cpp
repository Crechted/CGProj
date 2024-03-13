#include "Camera.h"

#include "../Components/MovementComponent.h"
#include "../Components/SceneComponent.h"
#include "../Core/Game.h"
#include "../Core/Input/InputDevice.h"
#include "Components/SpringArmComponent.h"

Camera::Camera(Vector3 initPosition, Vector3 initRotation)
    : initPosition(initPosition), initRotation(initRotation)
{
    sceneComp = CreateComponent<SceneComponent>();
    movementComp = CreateComponent<MovementComponent>();
    springArmComp = CreateComponent<SpringArmComponent>();
    movementComp->sceneComp = springArmComp;
    sceneComp->AttachTo(springArmComp);

    springArmComp->initPosition = initPosition;
    springArmComp->initRotation = initRotation;
    springArmComp->controlCamera = this;
    //sceneComp->initPosition = Vector3(0.0f, 0.0f, 0.0f);
    //sceneComp->initRotation = Vector3(0.0f);
}

void Camera::Reload()
{
    Object::Reload();
    scale = initScale;
}

void Camera::Draw()
{
    //Object::Draw();
}

void Camera::Initialize()
{
    Object::Initialize();
    scale = initScale;
    game->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Camera::OnKeyDown);
    game->GetInputDevice()->KeyUpDelegate.AddRaw(this, &Camera::OnKeyUp);
    game->GetInputDevice()->MouseMoveDelegate.AddRaw(this, &Camera::OnMouse);

    UpdateViewMatrix();
    UpdateProjMatrix();
}

void Camera::Update(float timeTick)
{
    Object::Update(timeTick);
    UpdateViewMatrix();
    UpdateProjMatrix();

    scale += delScale * timeTick;
}

void Camera::UpdateViewMatrix()
{
    const auto worldMat = sceneComp->GetWorldMatrix();
    const auto worldLoc = worldMat.Translation();
    const auto worldUp = worldMat.Up();
    const auto worldSpringMat = springArmComp->GetWorldMatrix();
    const auto worldSpringLoc = worldSpringMat.Translation();
    const auto tar = worldSpringLoc + worldSpringMat.Forward();
    mView = Matrix::CreateLookAt(worldLoc, tar, worldUp);

    /*auto Res = Matrix();
    Res = worldMat;
    printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f :: Rotation X=%04.4f Y=%04.4f Z=%04.4f \n",
        sceneComp->GetLocation().x,
        sceneComp->GetLocation().y,
        sceneComp->GetLocation().z,
        Res.Translation().x,
        Res.Translation().y,
        Res.Translation().z);*/
}

void Camera::UpdateProjMatrix()
{
    mProj = isPerspective
                ? Matrix::CreatePerspective(width * scale, height * scale, nearPlane, farPlane)
                : Matrix::CreateOrthographic(widthOrt * scale, heightOrt * scale, nearPlane, farPlane);
}

void Camera::OnKeyDown(Keys key)
{
    if (key == Keys::M) isPerspective = !isPerspective;
    if (key == Keys::R) Reload();
    if (key == Keys::OemPlus) delScale += speedScale;
    if (key == Keys::OemMinus) delScale -= speedScale;
}

void Camera::OnKeyUp(Keys key)
{
    if (key == Keys::OemPlus)
    {
        delScale += -speedScale;
        printf("Scale %f\n", scale);
    }
    if (key == Keys::OemMinus)
    {
        delScale -= -speedScale;
        printf("Scale %f\n", scale);
    }
}

void Camera::OnMouse(const MouseMoveEventArgs& mouseData)
{
}
