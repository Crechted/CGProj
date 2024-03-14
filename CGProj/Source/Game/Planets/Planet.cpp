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

void Planet::AddSputnik(Planet* dirPlanet)
{
    dirPlanet->meshComponent->attachOnlyTranslation = true;
    dirPlanet->parentPlanet = this;
    dirPlanet->meshComponent->AttachTo(meshComponent);
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
