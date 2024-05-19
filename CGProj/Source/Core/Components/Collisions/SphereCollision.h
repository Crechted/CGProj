#pragma once
#include "Core/Components/CollisionComponent.h"
#include "Game/Objects/Drawable/DrawSphere.h"

class SphereCollision : public CollisionComponent
{
public:
    SphereCollision(float radius = 1.0f);
    
    bool Contains(CollisionComponent* other) const override;
    void InitCollision() override;
    void UpdateCollision() override;

    DirectX::BoundingSphere sphereCollision;
protected:
    float radius;
};
