#pragma once
#include "Core/Objects/Object.h"

enum class Keys;
class SpringArmComponent;
class PlayerMoveComponent;
class SphereComponent;

class Pawn : public Object
{
public:
    Pawn();
    void Initialize() override;
    SpringArmComponent* GetSpringArmComponent() const { return springArmComp; }
    SphereComponent* GetSphereComponent() const { return sphereComp; }
    PlayerMoveComponent* GetPlayerMoveComponent() const { return playerMoveComp; }

    void Update(float timeTick) override;

protected:
    SpringArmComponent* springArmComp = nullptr;
    SphereComponent* sphereComp = nullptr;
    PlayerMoveComponent* playerMoveComp = nullptr;

    
    virtual void OnKeyDown(Keys key);
};
