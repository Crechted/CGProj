#pragma once
#include "RacketMoveComponent.h"

class Ball;

class RacketAIMoveComponent : public RacketMoveComponent
{
public:
    void Initialize() override;
    void Update(float timeTick) override;

    void GetBall();
    Ball* ball = nullptr;
};
