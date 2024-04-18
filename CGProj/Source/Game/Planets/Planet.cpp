#include "Planet.h"

#include "Game/Camera.h"
#include "Core/Engine.h"
#include "Core/Input/InputDevice.h"
#include "Game/Components/DrawSphereComponent.h"
#include "Game/Components/SpringArmComponent.h"
#include "Components/PlanetMoveComponent.h"

Planet::Planet()
{
    meshComponent = CreateComponent<DrawSphereComponent>();
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
    dirPlanet->meshComponent->attachRotate = false;
    dirPlanet->parentPlanet = this;
    dirPlanet->meshComponent->AttachTo(meshComponent);
}

void Planet::OnKeyDown(Keys key)
{
    if(key == keyToPoses)
    {
        for (const auto camera : engInst->GetCamerasOnViewport())
        {
            if (const auto springArmComp = dynamic_cast<SpringArmComponent*>(camera->GetTargetView()))
            {
                springArmComp->AttachTo(meshComponent);
                springArmComp->SetLocation(Vector3(0.0f));        
                springArmComp->SetRotation(Vector3(0.0f));
                springArmComp->springLenght = meshComponent->radius*4.0f;
            }
        }
    }
}
