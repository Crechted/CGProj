#include "BallMoveComponent.h"

#include <random>
using namespace DirectX::SimpleMath;

float GetRand()
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<float> dist(-1.0, 1.0);
    return dist(gen);
}

void BallMoveComponent::Initialize()
{
    CreateConstBuffer();
    /*velocity.x = GetRand();
    velocity.y = GetRand();*/
    velocity.x = -0.5;
    velocity.y = 0;
    velocity = XMVectorClamp(velocity, Vector3(-speed, -speed, -speed), Vector3(speed, speed, speed));
}

void BallMoveComponent::CalcOffset(float timeTick)
{
    velocity *= speed;
    offset.x += velocity.x * timeTick;
    offset.y += velocity.y * timeTick;
}
