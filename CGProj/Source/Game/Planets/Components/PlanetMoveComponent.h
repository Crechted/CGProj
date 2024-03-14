#pragma once
#include "../../../Components/MovementComponent.h"

class Planet;

class PlanetMoveComponent : public MovementComponent
{
public:
    PlanetMoveComponent()
    {
    }

    void Update(float timeTick) override;
    void Initialize() override;
    void HandleInputByKey() override;
    void CalcOffset(float timeTick) override;

    bool isRotationAround = false;
    Vector3 rotationAroundAxis = Vector3(10.0f, 0.0f, 0.0f);

    float orbitRadius;
    bool isMove = true;

private:
    Planet* ownerPlanet;
};
