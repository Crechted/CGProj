#include "Planet.h"

#include "../../Core/Game.h"
#include "../Components/BoxComponent.h"
#include "Components/PlanetMoveComponent.h"

Planet::Planet()
{
    moveComp = CreateComponent<PlanetMoveComponent>();
    meshComponent = CreateComponent<BoxComponent>();
}

void Planet::Initialize()
{
    Object::Initialize();
}

void Planet::AddSputnik(Planet* dirPlanet, float orbitRadius)
{
    meshComponent->childComponents.insert(dirPlanet->meshComponent);
    dirPlanet->meshComponent->parentComponent = meshComponent;
    dirPlanet->moveComp->orbitRadius = orbitRadius;
}

void Planet::AddSputnik(float orbitRadius)
{
    auto newPlanet = game->CreateObjects<Planet>();
    meshComponent->childComponents.insert(newPlanet->meshComponent);
    newPlanet->meshComponent->parentComponent = meshComponent;
    newPlanet->moveComp->orbitRadius = orbitRadius;
}
