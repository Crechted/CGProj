#pragma once
#include "SimpleMath.h"
#include "Core/Objects/Object.h"

enum class Keys;
class MovementComponent;
class SphereComponent;
class SpringArmComponent;
using namespace DirectX::SimpleMath;

class Spectator : public Object
{
public:
    Spectator(float lenght = 0.0f, Vector3 pos = Vector3(0.0f, 0.5f, 0.0f));
    void Initialize() override;
    SpringArmComponent* GetSpringArmComponent() const { return springArmComp; }
    MovementComponent* GetMovementComponent() const { return moveComp; }


protected:
    SpringArmComponent* springArmComp = nullptr;
    MovementComponent* moveComp = nullptr;

    
    virtual void OnKeyDown(Keys key);
};
