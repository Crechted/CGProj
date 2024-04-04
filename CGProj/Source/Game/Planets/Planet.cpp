#include "Planet.h"

#include "../Camera.h"
#include "..\..\Core\Engine.h"
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
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &Planet::OnKeyDown);
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
        for (const auto camera : engInst->GetCamerasOnViewport())
        {
            camera->springArmComp->AttachTo(meshComponent);
            camera->springArmComp->SetLocation(Vector3(0.0f));        
            camera->springArmComp->SetRotation(Vector3(0.0f));
            camera->springArmComp->springLenght = meshComponent->radius*4.0f;
        }
    }
}
