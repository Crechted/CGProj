#include "Camera.h"

#include "../Components/MovementComponent.h"
#include "../Components/SceneComponent.h"
#include "../Core/Game.h"
#include "../Core/WinDisplay.h"

Camera::Camera()
{    
    sceneComponent = CreateComponent<SceneComponent>();
    movementComponent = CreateComponent<MovementComponent>();
}

void Camera::Initialize()
{
    Object::Initialize();
    mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), sceneComponent->Forward(), sceneComponent->Up());
    farPlane = 1.0f;
    isPerspective = true;
    mProj = isPerspective ? Matrix::CreatePerspective(width, height, nearPlane, farPlane) : Matrix::CreateOrthographic(widthOrt, heightOrt, nearPlane, farPlane);
}

void Camera::Update(float timeTick)
{
    Object::Update(timeTick);
    mWorld = sceneComponent->mTransform;
    mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), sceneComponent->Forward(), sceneComponent->Up());
    //mView = Matrix::CreateLookAt(movementComponent->GetLocation(), movementComponent->curTarget, movementComponent->GetLocation()+movementComponent->mTransform.Up());
    
    /*printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f :: Rotation X=%04.4f Y=%04.4f Z=%04.4f \n",
        sceneComponent->GetLocation().x,
        sceneComponent->GetLocation().y,
        sceneComponent->GetLocation().z,
        sceneComponent->Forward().x,
        sceneComponent->Forward().y,
        sceneComponent->Forward().z);*/
}