#include "Camera.h"

#include "../Components/MovementComponent.h"
#include "../Components/SceneComponent.h"
#include "../Core/Game.h"

Camera::Camera(Vector3 initPosition, Vector3 initRotation)
{
    sceneComponent = new SceneComponent();
    sceneComponent->Owner = this;
    movementComponent = CreateComponent<MovementComponent>();
    movementComponent->sceneComp = sceneComponent;
    gameComponents.insert(sceneComponent);

    sceneComponent->initPosition = initPosition;
    sceneComponent->initRotation = initRotation;
}

void Camera::Draw()
{
    //Object::Draw();
}

void Camera::Initialize()
{
    Object::Initialize();
    const auto tar = sceneComponent->GetLocation() + sceneComponent->GetForward();
    mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), tar, sceneComponent->GetUp());
    farPlane = 1.0f;
    isPerspective = true;
    mProj = isPerspective
                ? Matrix::CreatePerspective(width, height, nearPlane, farPlane)
                : Matrix::CreateOrthographic(widthOrt, heightOrt, nearPlane, farPlane);

}

void Camera::Update(float timeTick)
{
    mProj = isPerspective
                ? Matrix::CreatePerspective(width, height, nearPlane, farPlane)
                : Matrix::CreateOrthographic(widthOrt, heightOrt, nearPlane, farPlane);
    Object::Update(timeTick);
    const auto tar = sceneComponent->GetLocation() + sceneComponent->GetForward();
    mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), tar,
        sceneComponent->GetUp());

    /*printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f :: Rotation X=%04.4f Y=%04.4f Z=%04.4f \n",
        sceneComponent->GetLocation().x,
        sceneComponent->GetLocation().y,
        sceneComponent->GetLocation().z,
        sceneComponent->GetUp().x,
        sceneComponent->GetUp().y,
        sceneComponent->GetUp().z);*/

}
