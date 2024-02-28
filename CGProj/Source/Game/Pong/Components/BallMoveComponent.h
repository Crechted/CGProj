#pragma once
#include "../../../Components/Movement2DComponent.h"

class BallMoveComponent : public Movement2DComponent
{
public:

    void Initialize() override;
    DirectX::SimpleMath::Vector2 velocity;

    void CalcOffset(float timeTick) override;
};
