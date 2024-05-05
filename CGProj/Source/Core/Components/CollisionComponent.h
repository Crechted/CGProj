#pragma once
#include "CollisionComponent.h"
#include "Utils/Delegates.h"
#include "Core/Components/SceneComponent.h"

class DrawComponent;

class CollisionComponent : public SceneComponent
{
public:
    CollisionComponent();

    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;
    virtual void InitCollision() = 0;
    virtual void UpdateCollision() = 0;

    void SetCollisionVisibility(bool visible) { showCollision = visible; }
    void SetCollisionEnabled(bool enable) { collisionEnabled = enable; }
    bool CollisionEnabled() const { return collisionEnabled; }

    virtual bool Contains(CollisionComponent* other) const = 0;
    const Array<CollisionComponent*>& GetOverlappedCollisions() const { return overlappedCollisions; }

    MulticastDelegate<CollisionComponent*> beginOverlapped;
    MulticastDelegate<CollisionComponent*> endOverlapped;

protected:
    DrawComponent* debugCollision = nullptr;

    Array<CollisionComponent*> overlappedCollisions;

    bool showCollision = false;
    bool collisionEnabled = true;
    void OnBeginOverlap(CollisionComponent* other);
    void OnEndOverlap(CollisionComponent* other);
};
