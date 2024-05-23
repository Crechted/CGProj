#include "BallMoveComponent.h"
#include "Utils/HelperFunctions.h"

using namespace DirectX::SimpleMath;

float BallMoveComponent::GetRand() const
{
    return Rand(-speed, speed);
}

void BallMoveComponent::Initialize()
{
    CreateConstBuffer();
    InitStartVelocity();
}

void BallMoveComponent::InitStartVelocity()
{
    velocity.x = GetRand();
    velocity.y = GetRand();
    const float minSpeedX = velocity.x < 0 ? -speed : speed/2;
    const float minSpeedY = velocity.y < 0 ? -speed/2 : 0;
    const float maxSpeedX = velocity.x < 0 ? -speed/2 : speed;
    const float maxSpeedY = velocity.y < 0 ? 0 : speed/2;
    velocity = XMVectorClamp(velocity, Vector3(minSpeedX, minSpeedY, -speed), Vector3(maxSpeedX, maxSpeedY, speed));
}

void BallMoveComponent::Reload()
{
    Movement2DComponent::Reload();
    InitStartVelocity();
}

void BallMoveComponent::CalcOffset(float timeTick)
{
    //velocity *= speed;
    offset.x += velocity.x * timeTick;
    offset.y += velocity.y * timeTick;
}
