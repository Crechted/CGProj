#include "Camera.h"

#include "Core/Components/SceneComponent.h"
#include "Core/Engine.h"
#include "Core/Input/InputDevice.h"
#include "Utils/HelperFunctions.h"


Camera::Camera(Vector3 initPosition, Vector3 initRotation)
    : initPosition(initPosition), initRotation(initRotation)
{
    sceneComp = CreateComponent<SceneComponent>();
    eyeData.isCam = true;
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
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Camera::OnKeyDown);
    engInst->GetInputDevice()->KeyUpDelegate.AddRaw(this, &Camera::OnKeyUp);
    engInst->GetInputDevice()->MouseMoveDelegate.AddRaw(this, &Camera::OnMouse);

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
    Matrix worldMat = sceneComp->GetWorldMatrix();

    const Matrix targetMat = targetView ? targetView->GetWorldMatrix() : sceneComp->GetWorldMatrix();
    const Vector3 targetLoc = targetMat.Translation();

    const Vector3 loc = worldMat.Translation();
    Vector3 tar, up;

    if (viewType == ViewType::General || viewType == ViewType::Perspective || viewType == ViewType::OrtFree)
    {
        //loc = worldMat.Translation();
        tar = targetLoc + targetMat.Forward();
        up = sceneComp->GetUp();
    }

    if (viewType == ViewType::OrtXOZ)
    {
        //loc = worldMat.Translation();
        tar = loc + Vector3(0.0f, -1.0f, 0.0f);
        up = Vector3(-1.0f, 0.0f, 0.0f);
    }

    if (viewType == ViewType::OrtYOZ)
    {
        //loc = worldMat.Translation();
        tar = loc + Vector3(-1.0f, 0.0f, 0.0f);
        up = Vector3(0.0f, 1.0f, 0.0f);
    }

    if (viewType == ViewType::OrtXOY)
    {
        //loc = worldMat.Translation();
        tar = loc + Vector3(0.0f, 0.0f, -1.0f);
        up = Vector3(0.0f, 1.0f, 0.0f);
    }
    
    eyeData.mView = Matrix::CreateLookAt(loc, tar, up);
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
    if (viewType == ViewType::General)
    {
        eyeData.mProj = isPerspective
                            ? Matrix::CreatePerspective(width * scale, height * scale, nearPlane, farPlane)
                            : Matrix::CreateOrthographic(widthOrt * scale, heightOrt * scale, nearPlane, farPlane);
    }
    else if (viewType == ViewType::Perspective)
        //eyeData.mProj = Matrix::CreatePerspective(width * scale, height * scale, nearPlane, farPlane);
        eyeData.mProj = Matrix::CreatePerspectiveFieldOfView(RadiansFromDegree(75) * scale, 1.0f, nearPlane, farPlane);
    else eyeData.mProj = Matrix::CreateOrthographic(widthOrt * scale, heightOrt * scale, nearPlane, farPlane);
}

void Camera::OnKeyDown(Keys key)
{
    if (key == Keys::P) isPerspective = true;
    if (key == Keys::O) isPerspective = false;

    if (key == Keys::R) Reload();
    if (key == Keys::OemPlus) delScale += speedScale;
    if (key == Keys::OemMinus) delScale -= speedScale;
}

void Camera::OnKeyUp(Keys key)
{
    if (key == Keys::OemPlus)
    {
        delScale -= speedScale;
        printf("Scale %f\n", scale);
    }
    if (key == Keys::OemMinus)
    {
        delScale += speedScale;
        printf("Scale %f\n", scale);
    }
}

void Camera::OnMouse(const MouseMoveEventArgs& mouseData)
{
}

void Camera::GetProjAndDistanceBySection(float nearLayer, float farLayer, Matrix& retProj, float& retDistanceToFar)
{
    nearLayer = nearLayer >= 0.0f && nearLayer <= 1.0f ? nearPlane + (farPlane - nearPlane) * nearLayer : nearPlane;
    farLayer = farLayer >= 0.0f && farLayer <= 1.0f ? nearPlane + (farPlane - nearPlane) * farLayer : farPlane;
    retDistanceToFar = farLayer;
    if (viewType == ViewType::General)
    {
        retProj = isPerspective
                      ? Matrix::CreatePerspective(width * scale, height * scale, nearLayer, farLayer)
                      : Matrix::CreateOrthographic(widthOrt * scale, heightOrt * scale, nearLayer, farLayer);
    }
    if (viewType == ViewType::Perspective)
        //return Matrix::CreatePerspective(width * scale, height * scale, nearLayer, farLayer);
        retProj = Matrix::CreatePerspectiveFieldOfView(RadiansFromDegree(75) * scale, 1.0f, nearLayer, farLayer);
    else
        retProj = Matrix::CreateOrthographic(widthOrt * scale, heightOrt * scale, nearLayer, farLayer);
}
