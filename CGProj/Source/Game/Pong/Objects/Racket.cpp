#include "Racket.h"

#include "Wall.h"
#include "../../../Components/TriangleComponent.h"
#include "../Components/RacketMoveComponent.h"

Racket::Racket(Vector3 center, Vector3 extends, Vector3 color)
{
    this->center = center;
    this->extends = extends;
    this->color = color;
}

void Racket::CreateComponents()
{
    moveComp = new RacketMoveComponent();
    boxSprite = new TriangleComponent();
}

void Racket::OnBeginOverlapped(Object* other)
{
    const auto ballMoveComp = dynamic_cast<RacketMoveComponent*>(moveComp);
    if (!ballMoveComp || ballMoveComp->bWasBlockedUp || ballMoveComp->bWasBlockedDown) return;

    if (const Wall* wall = dynamic_cast<Wall*>(other))
    {
        if (wall->type == WallType::Up) ballMoveComp->bWasBlockedUp = true;
        if (wall->type == WallType::Down) ballMoveComp->bWasBlockedDown = true;
        ballMoveComp->Stop();
    }
}

void Racket::OnEndOverlapped(Object* other)
{
    GameSquare::OnEndOverlapped(other);
    const auto ballMoveComp = dynamic_cast<RacketMoveComponent*>(moveComp);
    if (!ballMoveComp || (!ballMoveComp->bWasBlockedUp && !ballMoveComp->bWasBlockedDown)) return;

    if (const Wall* wall = dynamic_cast<Wall*>(other))
    {
        if (wall->type == WallType::Up) ballMoveComp->bWasBlockedUp = false;
        if (wall->type == WallType::Down) ballMoveComp->bWasBlockedDown = false;
    }
}
