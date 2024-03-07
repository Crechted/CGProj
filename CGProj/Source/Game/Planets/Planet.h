#pragma once
#include "../../Core/Object.h"

class PlanetMoveComponent;
class BoxComponent;

class Planet : public Object
{
public:

    Planet();

    void Initialize() override;
    BoxComponent* meshComponent;
    PlanetMoveComponent* moveComp;

    void AddSputnik(Planet* dirPlanet, float OrbitRadius);
    void AddSputnik(float OrbitRadius);
};
