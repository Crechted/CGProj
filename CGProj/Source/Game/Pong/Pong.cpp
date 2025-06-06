#include "Pong.h"

#include "Components/Movement2DComponent.h"
#include "..\..\Core\Engine.h"
#include "Objects/Ball.h"
#include "Objects/Racket.h"

void Pong::Initialize()
{
    engInst->Initialize();
    AddRacket(engInst, rightIsAI, 1.5f, {0.9f, 0.0f, 0.5f}, {0.015f, 0.15f, 0.2f}, {1.0f, 1.0f, 1.0f}, Keys::Up, Keys::Down, Keys::Left, Keys::Right);
    AddRacket(engInst, leftIsAI, 1.0f, {-0.9f, -0.0f, 0.5f}, {0.015f, 0.15f, 0.2f}, {1.0f, 0.0f, 0.0f});
    AddBall(engInst, {0.0f, 0.0f, 0.5f}, {0.02f, 0.02f, 0.2f}, {1.0f, 1.0f, 1.0f}, 2.0f);
    AddWall(engInst, WallType::Up, {0.0f, 0.5f, 0.5f});
    AddWall(engInst, WallType::Down, {0.0f, -0.5f, 0.5f});
    AddWall(engInst, WallType::Left);
    AddWall(engInst, WallType::Right);
}

void Pong::Run()
{
    engInst->Run();
}

void Pong::AddWall(Engine* engInst, WallType type)
{
    const auto wall = new Wall(type);
    wall->pong = this;
    wall->Initialize();
   // engInst->gameObjects.insert(wall);
}

void Pong::AddWall(Engine* engInst, WallType type, Vector3 center,
        Vector3 extends,
        Vector3 Color)
{
    const auto wall = new Wall(type);
    wall->pong = this;
    wall->center = center;
    wall->extends = extends;
    wall->color = Color;
    wall->Initialize();
   // engInst->gameObjects.insert(wall);
}

void Pong::AddRacket(Engine* engInst, bool isAI, float speed, Vector3 center, Vector3 extends, Vector3 Color, Keys up, Keys down, Keys left, Keys right)
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
   // engInst->gameObjects.insert(racket);
}

void Pong::AddRacket(Engine* engInst, bool isAI, float speed)
{
    const auto racket = new Racket();
    racket->isAI = isAI;
    racket->pong = this;
    racket->Initialize();
    racket->moveComp->speed = speed;
   // engInst->gameObjects.insert(racket);
}

void Pong::AddBall(Engine* engInst, Vector3 center, Vector3 extends, Vector3 Color, float speed)
{
    const auto ball = new Ball(center, extends, Color);
    ball->pong = this;
    ball->Initialize();
    ball->moveComp->speed = speed;
   // engInst->gameObjects.insert(ball);
}

void Pong::IncreaseScoreByWall(WallType wall)
{
    if (wall == WallType::Left) score.y++;
    if (wall == WallType::Right) score.x++;

    printf("SCORE: %d : %d\n", static_cast<int32_t>(score.x), static_cast<int32_t>(score.y));
}
