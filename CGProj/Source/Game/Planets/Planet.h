#pragma once
#include "Core/Object.h"
#include "Core/Input/Keys.h"

class PlanetMoveComponent;
class SphereComponent;

class Planet : public Object
{
public:

    Planet();

    void Initialize() override;
    SphereComponent* meshComponent;
    PlanetMoveComponent* moveComp;

    Planet* parentPlanet = nullptr;

    Keys keyToPoses = Keys::None;
    
    void AddSputnik(Planet* dirPlanet);

protected:
    void OnKeyDown(Keys key);
};
