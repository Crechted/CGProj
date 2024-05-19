#pragma once
#include "Core/Components/CollisionComponent.h"
#include "Game/Objects/Drawable/Box.h"

class BoxCollision : public CollisionComponent
{
public:
    BoxCollision(Vector3 size = Vector3::One);

    bool Contains(CollisionComponent* other) const override;
    void InitCollision() override;
    void UpdateCollision() override;

    DirectX::BoundingBox boxCollision;
protected:
    Vector3 size;
};
