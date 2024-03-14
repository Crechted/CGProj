#include "PlanetMoveComponent.h"

#include "../Planet.h"
#include "../../../Components/SceneComponent.h"
#include "../../../Core/Game.h"
#include "../../Components/SphereComponent.h"

void PlanetMoveComponent::Update(float timeTick)
{
    MovementComponent::Update(timeTick);
}

void PlanetMoveComponent::Initialize()
{
    MovementComponent::Initialize();
    if (!sceneComp && !SearchSceneComponent()) return;
    if (!game || !sceneComp) return;
    ownerPlanet = reinterpret_cast<Planet*>(Owner);
    Reload();
}

void PlanetMoveComponent::HandleInputByKey()
{
    //MovementComponent::HandleInputByKey();
}

void PlanetMoveComponent::CalcOffset(float timeTick)
{
    if (!sceneComp) return;

    if ( isMove && ownerPlanet && ownerPlanet->parentPlanet)
    {
        auto parentScene = ownerPlanet->parentPlanet->meshComponent;
        auto parentWorldMat = parentScene->GetWorldMatrix();
        auto parentWorldLoc = parentWorldMat.Translation();
        auto worldMat = sceneComp->GetWorldMatrix();
        auto worldLoc = worldMat.Translation();
        Vector3 gravityVelocity = parentWorldLoc - worldLoc;
        gravityVelocity.Normalize();
        auto velocity = gravityVelocity.Cross(sceneComp->GetUp());
        sceneComp->AddLocation(velocity * speed * timeTick);
        /*printf(" Gravity: X=%04.4f Y:%04.4f Z:%04.4f\n",
            velocity.x,
            velocity.y,
            velocity.z);*/
    }

    if (isRotationAround)
    {
        sceneComp->AddRotation(rotationAroundAxis * speed * timeTick);
        /*printf(" Rotation: Pitch=%04.4f Yaw:%04.4f Roll:%04.4f\n",
            sceneComp->GetRotation().x,
            sceneComp->GetRotation().y,
            sceneComp->GetRotation().z);*/
    }
}
