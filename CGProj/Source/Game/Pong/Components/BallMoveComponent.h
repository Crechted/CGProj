#pragma once
#include "../../../Components/Movement2DComponent.h"

class BallMoveComponent : public Movement2DComponent
{
public:

    float GetRand() const;
    void Initialize() override;
    DirectX::SimpleMath::Vector2 velocity;
    void Reload() override;
    void InitStartVelocity();

    void CalcOffset(float timeTick) override;
};
