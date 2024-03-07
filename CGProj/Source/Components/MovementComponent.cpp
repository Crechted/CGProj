#include "MovementComponent.h"

#include "SceneComponent.h"
#include "../Core/Game.h"
#include "../Core/Input/InputDevice.h"

static constexpr float PI = 3.14159265f;

bool MovementComponent::SearchSceneComponent()
{
    if (Owner)
    {
        for (const auto comp : Owner->gameComponents)
        {
            if (const auto sceneC = reinterpret_cast<SceneComponent*>(comp))
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
    if (!SearchSceneComponent()) return;
    if (!game || !sceneComp) return;
    Reload();
    game->inputDevice->KeyDownDelegate.AddRaw(this, &MovementComponent::OnKeyDown);
    game->inputDevice->KeyUpDelegate.AddRaw(this, &MovementComponent::OnKeyUp);
    game->inputDevice->MouseMoveDelegate.AddRaw(this, &MovementComponent::OnMouseMove);
    sceneComp->mTransform = Matrix::CreateWorld(sceneComp->location, sceneComp->forward, sceneComp->up);
}

void MovementComponent::Reload()
{
    /*offsetLocation = Vector4::Zero;
    offsetRotation = Vector4::Zero;*/
    if(!sceneComp) return;
    sceneComp->location = sceneComp->initPosition;
    //location = mTransform.Translation();
    sceneComp->rotation = sceneComp->initRotation;
    sceneComp->mTransform = Matrix::CreateWorld(sceneComp->location, sceneComp->forward, sceneComp->up);
}

void MovementComponent::Update(float timeTick)
{
    if(!sceneComp) return;
    HandleInputByKey();
    CalcOffset(timeTick);
}

void MovementComponent::HandleInputByKey()
{
    if(!sceneComp) return;

    delLocation = Vector3(0.0);
    if (game->inputDevice->IsKeyDown(inputForward)) delLocation += sceneComp->mTransform.Forward() * speed;
    if (game->inputDevice->IsKeyDown(inputRight)) delLocation += sceneComp->mTransform.Right() * speed;
    if (game->inputDevice->IsKeyDown(inputUp)) delLocation += sceneComp->up * speed;
    if (game->inputDevice->IsKeyDown(inputBackward)) delLocation += -sceneComp->mTransform.Forward() * speed;
    if (game->inputDevice->IsKeyDown(inputLeft)) delLocation += -sceneComp->mTransform.Right() * speed;
    if (game->inputDevice->IsKeyDown(inputDown)) delLocation += -sceneComp->up * speed;
}

void MovementComponent::CalcOffset(float timeTick)
{
    if(!sceneComp) return;

    /*if (rotation.x * timeTick > PI / 2 || rotation.x * timeTick < -PI / 2)
    {
        /*printf(" Position: posX=%04.4f posY:%04.4f posZ:%04.4f\n",
                mTransform.Translation().x,
                mTransform.Translation().y,
                mTransform.Translation().z);#1#
        rotation -= delRotation;
        delRotation.x = 0;
    }*/

    Vector3 prevTrons = sceneComp->mTransform.Translation();
    sceneComp->mTransform *= Matrix::CreateTranslation(-prevTrons);

    
    //if (delRotation.x != 0 || delRotation.y != 0) sceneComp->mTransform *= Matrix::CreateFromAxisAngle(Vector3(-delRotation.x,-delRotation.y, 0.0),  timeTick);

    //sceneComp->mTransform *= Matrix::CreateFromAxisAngle(Vector3(0.1,0.0, 0.0),  timeTick);
    //sceneComp->mTransform *= Matrix::CreateFromQuaternion(Quaternion(-delRotation * timeTick, 1));
    sceneComp->mTransform *= Matrix::CreateRotationY(-delRotation.y * timeTick);
    sceneComp->mTransform *= sceneComp->mTransform.CreateRotationX(-delRotation.x * timeTick);
    //sceneComp->mTransform *= Matrix::CreateRotationZ(-delRotation.z * timeTick);
    sceneComp->mTransform *= Matrix::CreateTranslation(prevTrons);
    delRotation = Vector3(0.0);

    sceneComp->mTransform *= Matrix::CreateTranslation(delLocation * timeTick);

    /*printf(" Position: posX=%04.4f posY:%04.4f offsetZ:%04.4f\n",
        mTransform.Forward().x,
        mTransform.Forward().y,
        mTransform.Forward().z);*/
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
    if(!sceneComp) return;

    if (game->inputDevice->IsKeyDown(Keys::LeftShift)) return;
    delRotation.x = /*(1.0f-fmod(sceneComp->Forward().z, 1.0f)) * */mouseEvent.Offset.y * sensitive;
    //delRotation.z = fmod(sceneComp->Forward().z, 1.0f)*mouseEvent.Offset.y * sensitive;
    delRotation.y = mouseEvent.Offset.x * sensitive;
    sceneComp->rotation += delRotation;
}
