#pragma once
#include "Core/Objects/Object.h"
#include "Core/Input/Keys.h"

class PlanetMoveComponent;
class DrawSphereComponent;

class Planet : public Object
{
public:

    Planet();

    void Initialize() override;
    DrawSphereComponent* meshComponent;
    PlanetMoveComponent* moveComp;

    Planet* parentPlanet = nullptr;

    Keys keyToPoses = Keys::None;
    
    void AddSputnik(Planet* dirPlanet);

protected:
    void OnKeyDown(Keys key);
};
