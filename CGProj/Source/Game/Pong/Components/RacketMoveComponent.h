#pragma once
#include "Movement2DComponent.h"

class RacketMoveComponent : public Movement2DComponent
{
public:

    bool bWasBlockedUp = false;
    bool bWasBlockedDown = false;
    void OnKeyDown(Keys key) override;
    void OnKeyUp(Keys key) override;
    void AddInputByKey(Keys key, float vel) override;
    void Stop();
protected:
    void CalcOffset(float timeTick) override;
};
