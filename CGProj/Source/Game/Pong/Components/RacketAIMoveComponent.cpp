#include "RacketAIMoveComponent.h"

#include "..\..\..\Core\Engine.h"
#include "../Objects/Ball.h"

void RacketAIMoveComponent::Initialize()
{
    if (!engInst) return;

    CreateConstBuffer();
}

void RacketAIMoveComponent::Update(float timeTick)
{
    if (!ball) GetBall();
    if (ball->curCenter.y < offset.y) delPosition.y = -speed;
    if (ball->curCenter.y > offset.y) delPosition.y = speed;
    RacketMoveComponent::Update(timeTick);
}

void RacketAIMoveComponent::GetBall()
{
    if (!engInst) return;
    /*for (const auto Obj : engInst->gameObjects)
    {
        if (const auto oBall = dynamic_cast<Ball*>(Obj))
        {
            ball = oBall;
            return;
        }
    }*/
}
