#pragma once
#include "SimpleMath.h"
#include "Core/Objects/Object.h"

enum class Keys;
class SpringArmComponent;
class PlayerMoveComponent;
class SphereComponent;
class CollisionComponent;

class CatamariPawn : public Object
{
public:
    CatamariPawn(float lenght = 5.0f, DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(0.0f, 0.5f, 0.0f));
    void Initialize() override;
    SpringArmComponent* GetSpringArmComponent() const { return springArmComp; }
    SphereComponent* GetSphereComponent() const { return sphereComp; }
    PlayerMoveComponent* GetPlayerMoveComponent() const { return playerMoveComp; }

protected:
    SpringArmComponent* springArmComp = nullptr;
    SphereComponent* sphereComp = nullptr;
    PlayerMoveComponent* playerMoveComp = nullptr;

    void OnBeginOverlap(CollisionComponent* other);
    virtual void OnKeyDown(Keys key);
};