#include "RacketMoveComponent.h"

using namespace DirectX::SimpleMath;

void RacketMoveComponent::OnKeyDown(Keys key)
{
    Movement2DComponent::OnKeyDown(key);
}

void RacketMoveComponent::OnKeyUp(Keys key)
{
    if (!(bWasBlockedUp || bWasBlockedDown)) Movement2DComponent::OnKeyUp(key);
}

void RacketMoveComponent::AddInputByKey(Keys key, float vel)
{
    if (key == inputUp)
    {
        if (bWasBlockedUp)
            delPosition.y += 0;
        else
        {
            delPosition.y += vel;
        }
    }
    //if (key == inputLeft) delPosition.x -= vel;
    if (key == inputDown)
    {
        if (bWasBlockedDown)
            delPosition.y -= 0;
        else
        {
            delPosition.y -= vel;
        }
    }
    //if (key == inputRight) delPosition.x += vel;
}

void RacketMoveComponent::Stop()
{
    delPosition = {0};
}


void RacketMoveComponent::CalcOffset(float timeTick)
{
    Movement2DComponent::CalcOffset(timeTick);
    offset = XMVectorClamp(offset, Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 1.0));
}
