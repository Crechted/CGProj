#pragma once
#include "../../../Components/MovementComponent.h"

class PlanetMoveComponent : public MovementComponent
{
public:
PlanetMoveComponent(){}
    void Update(float timeTick) override;
    void Initialize() override;
    virtual void HandleInputByKey();
    virtual void CalcOffset(float timeTick);

    bool isRotationAround = false;
    Vector3 rotationAroundAxis = Vector3(0.002f, 0.001f, 0.0f);

    float orbitRadius;
};
