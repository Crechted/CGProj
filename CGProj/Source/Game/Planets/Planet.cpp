#include "Planet.h"

#include "../../Core/Game.h"
#include "../Components/BoxComponent.h"
#include "Components/PlanetMoveComponent.h"

Planet::Planet()
{
    meshComponent = new BoxComponent();
    meshComponent->Owner = this;
    moveComp = CreateComponent<PlanetMoveComponent>();
    moveComp->sceneComp = meshComponent;
    gameComponents.insert(meshComponent);
}

void Planet::Initialize()
{
    Object::Initialize();
}

void Planet::AddSputnik(Planet* dirPlanet, float orbitRadius)
{
    dirPlanet->meshComponent->parentComponent = meshComponent;
    dirPlanet->moveComp->orbitRadius = orbitRadius;
}

void Planet::AddSputnik(float orbitRadius)
{
    auto newPlanet = game->CreateObjects<Planet>();
    newPlanet->meshComponent->parentComponent = meshComponent;
    newPlanet->moveComp->orbitRadius = orbitRadius;
}
