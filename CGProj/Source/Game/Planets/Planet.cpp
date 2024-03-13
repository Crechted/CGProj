#include "Planet.h"

#include "../Camera.h"
#include "../../Core/Game.h"
#include "../../Core/Input/InputDevice.h"
#include "../Components/SphereComponent.h"
#include "../Components/SpringArmComponent.h"
#include "Components/PlanetMoveComponent.h"

Planet::Planet()
{
    meshComponent = CreateComponent<SphereComponent>();
    moveComp = CreateComponent<PlanetMoveComponent>();
    moveComp->sceneComp = meshComponent;
}

void Planet::Initialize()
{
    Object::Initialize();
    game->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Planet::OnKeyDown);
}

void Planet::AddSputnik(Planet* dirPlanet, float orbitRadius)
{
    //irPlanet->meshComponent->AttachTo(meshComponent);
    dirPlanet->moveComp->orbitRadius = orbitRadius;
    dirPlanet->moveComp->isRotationAround = true;
    dirPlanet->moveComp->rotationAroundAxis = Vector3(2.0f, 20.0f, 0.0f);
    dirPlanet->parentPlanet = this;
}

void Planet::AddSputnik(float orbitRadius, Keys keyToPos)
{
    auto newPlanet = game->CreateObject<Planet>();
    newPlanet->meshComponent->radius = orbitRadius / 10.0f;
    newPlanet->meshComponent->initPosition = meshComponent->initPosition + Vector3(orbitRadius, orbitRadius, 0.0f);
    newPlanet->meshComponent->drawSecondHalf = false;

    newPlanet->moveComp->orbitRadius = orbitRadius;
    newPlanet->moveComp->isRotationAround = true;
    //newPlanet->moveComp->isMoveByDirection = true;
    newPlanet->moveComp->rotationAroundAxis = Vector3(2.0f, 20.0f, 0.0f);
    newPlanet->parentPlanet = this;

    newPlanet->keyToPoses = keyToPos;
}

void Planet::OnKeyDown(Keys key)
{
    if(key == keyToPoses)
    {
        game->GetCamera()->springArmComp->AttachTo(meshComponent);
        game->GetCamera()->springArmComp->SetLocation(Vector3(0.0f));        
        game->GetCamera()->springArmComp->SetRotation(Vector3(0.0f));
        game->GetCamera()->springArmComp->springLenght = meshComponent->radius*4.0f;
    }
}
