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
    if (!sceneComp && !SearchSceneComponent()) return;
    if (!game || !sceneComp) return;
    Reload();
}

void PlanetMoveComponent::HandleInputByKey()
{
    //MovementComponent::HandleInputByKey();
}

void PlanetMoveComponent::CalcOffset(float timeTick)
{
    if (!sceneComp) return;

    if (isRotationAround)
    {
        sceneComp->AddRotation(rotationAroundAxis * speed * timeTick);
        printf(" Rotation: Pitch=%04.4f Yaw:%04.4f Roll:%04.4f\n",
            sceneComp->GetRotation().x,
            sceneComp->GetRotation().y,
            sceneComp->GetRotation().z);
    }
    if (isMoveByVector)
    {
        sceneComp->AddLocation(moveByVector * speed * timeTick);
    }
}
