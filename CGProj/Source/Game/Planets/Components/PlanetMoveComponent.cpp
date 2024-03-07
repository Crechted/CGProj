#include "PlanetMoveComponent.h"

#include "../../../Components/SceneComponent.h"
#include "../../../Core/Game.h"

void PlanetMoveComponent::Update(float timeTick)
{
    MovementComponent::Update(timeTick);
}

void PlanetMoveComponent::Initialize()
{
    MovementComponent::Initialize();
    if (!SearchSceneComponent()) return;
    if (!game || !sceneComp) return;
    Reload();
    sceneComp->mTransform = Matrix::CreateWorld(sceneComp->location, sceneComp->forward, sceneComp->up);
}

void PlanetMoveComponent::HandleInputByKey()
{
    MovementComponent::HandleInputByKey();
}

void PlanetMoveComponent::CalcOffset(float timeTick)
{
    Vector3 prevTrons = sceneComp->mTransform.Translation();
    
    if (true)
    {
        sceneComp->mTransform *= Matrix::CreateTranslation(-prevTrons);        
        sceneComp->mTransform *= Matrix::CreateFromAxisAngle(-rotationAroundAxis, timeTick);
        //sceneComp->mTransform *= Matrix::CreateRotationY(-rotationAroundAxisy * timeTick);
        //sceneComp->mTransform *= sceneComp->mTransform.CreateRotationX(-delRotation.x * timeTick);
        sceneComp->mTransform *= Matrix::CreateTranslation(prevTrons);
    }
    //delLocation
    sceneComp->mTransform *= Matrix::CreateTranslation(delLocation * timeTick);
}
