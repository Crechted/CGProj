#include "CollisionComponent.h"

#include "DrawComponent.h"


CollisionComponent::CollisionComponent()
{
    debugCollision = CreateComponent<DrawComponent>();
}

void CollisionComponent::Initialize()
{
    InitCollision();
    beginOverlapped.AddRaw(this, &CollisionComponent::OnBeginOverlap);
    endOverlapped.AddRaw(this, &CollisionComponent::OnEndOverlap);
    SceneComponent::Initialize();
}

void CollisionComponent::Update(float timeTick)
{
    UpdateCollision();
    SceneComponent::Update(timeTick);
}

void CollisionComponent::OnBeginOverlap(CollisionComponent* other)
{
    if (other) overlappedCollisions.insert(other);
    printf("Overlaped!");
}

void CollisionComponent::OnEndOverlap(CollisionComponent* other)
{
    if (other) overlappedCollisions.remove(other);
    printf("Endlaped!");
}
