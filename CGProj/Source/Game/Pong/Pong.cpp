#include "Pong.h"

#include "Components/Movement2DComponent.h"
#include "../../Core/Game.h"
#include "Objects/Ball.h"
#include "Objects/Racket.h"

void Pong::Initialize()
{
    game->Initialize();
    AddRacket(game, rightIsAI, 1.5, {0.9, 0.0, 0.5}, {0.015, 0.15, 0.2}, {1.0, 1.0, 1.0}, Keys::Up, Keys::Down, Keys::Left, Keys::Right);
    AddRacket(game, leftIsAI, 1.0, {-0.9, -0.0, 0.5}, {0.015, 0.15, 0.2}, {1.0, 0.0, 0.0});
    AddBall(game, {0.0, 0.0, 0.5}, {0.02, 0.02, 0.2}, {1.0, 1.0, 1.0}, 2.0);
    AddWall(game, WallType::Up, {0.0, 0.5, 0.5});
    AddWall(game, WallType::Down, {0.0, -0.5, 0.5});
    AddWall(game, WallType::Left);
    AddWall(game, WallType::Right);
}

void Pong::Run()
{
    game->Run();
}

void Pong::AddWall(Game* game, WallType type)
{
    const auto wall = new Wall(type);
    wall->pong = this;
    wall->Initialize();
   // game->gameObjects.insert(wall);
}

void Pong::AddWall(Game* game, WallType type, Vector3 center,
        Vector3 extends,
        Vector3 Color)
{
    const auto wall = new Wall(type);
    wall->pong = this;
    wall->center = center;
    wall->extends = extends;
    wall->color = Color;
    wall->Initialize();
   // game->gameObjects.insert(wall);
}

void Pong::AddRacket(Game* game, bool isAI, float speed, Vector3 center, Vector3 extends, Vector3 Color, Keys up, Keys down, Keys left, Keys right)
{
    const auto racket = new Racket(center, extends, Color);
    racket->isAI = isAI;
    racket->pong = this;
    racket->Initialize();
    racket->moveComp->inputUp = up;
    racket->moveComp->inputDown = down;
    racket->moveComp->inputLeft = left;
    racket->moveComp->inputRight = right;
    racket->moveComp->speed = speed;
   // game->gameObjects.insert(racket);
}

void Pong::AddRacket(Game* game, bool isAI, float speed)
{
    const auto racket = new Racket();
    racket->isAI = isAI;
    racket->pong = this;
    racket->Initialize();
    racket->moveComp->speed = speed;
   // game->gameObjects.insert(racket);
}

void Pong::AddBall(Game* game, Vector3 center, Vector3 extends, Vector3 Color, float speed)
{
    const auto ball = new Ball(center, extends, Color);
    ball->pong = this;
    ball->Initialize();
    ball->moveComp->speed = speed;
   // game->gameObjects.insert(ball);
}

void Pong::IncreaseScoreByWall(WallType wall)
{
    if (wall == WallType::Left) score.y++;
    if (wall == WallType::Right) score.x++;

    printf("SCORE: %d : %d\n", static_cast<int32_t>(score.x), static_cast<int32_t>(score.y));
}
