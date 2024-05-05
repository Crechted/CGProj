#include "CollisionComponent.h"

#include "DrawComponent.h"


CollisionComponent::CollisionComponent()
{
    debugCollision = CreateComponent<DrawComponent>();
}

void CollisionComponent::Initialize()
{
    InitCollision();
    SceneComponent::Initialize();
    beginOverlapped.AddRaw(this, &CollisionComponent::OnBeginOverlap);
    endOverlapped.AddRaw(this, &CollisionComponent::OnEndOverlap);
}

void CollisionComponent::Update(float timeTick)
{
    SceneComponent::Update(timeTick);
    UpdateCollision();
}

void CollisionComponent::Draw()
{
    if (showCollision) SceneComponent::Draw();
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
