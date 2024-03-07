#include "Ball.h"

#include "Racket.h"
#include "Wall.h"
#include "../Pong.h"
#include "../../../Components/TriangleComponent.h"
#include "../Components/BallMoveComponent.h"

using namespace DirectX::SimpleMath;

Ball::Ball(Vector3 center, Vector3 extends, Vector3 color)
{
    this->center = center;
    this->extends = extends;
    this->color = color;
}


void Ball::CreateComponents()
{
    moveComp = CreateComponent<BallMoveComponent>();
    boxSprite = CreateComponent<TriangleComponent>();
}


void Ball::OnBeginOverlapped(Object* other)
{
    const auto ballMoveComp = dynamic_cast<BallMoveComponent*>(moveComp);
    if (!ballMoveComp) return;

    if (const Racket* racket = dynamic_cast<Racket*>(other))
    {
        ballMoveComp->velocity.x *= -1;
        ballMoveComp->velocity.y += curCenter.y - racket->curCenter.y;
    }
    if (const Wall* wall = dynamic_cast<Wall*>(other))
    {
        if (wall->type == WallType::Up || wall->type == WallType::Down)
            ballMoveComp->velocity.y *= -1;
        if (wall->type == WallType::Left || wall->type == WallType::Right)
        {

            if (pong) pong->IncreaseScoreByWall(wall->type);

            moveComp->Reload();
        }
    }
    //moveComp->speed *= 1.5;
}

void Ball::Update(float timeTick)
{
    if (curCenter.x < -1.0 || curCenter.x > 1.0 || curCenter.y < -1.0 || curCenter.y > 1.0) moveComp->Reload();
    GameSquare::Update(timeTick);
}
