#pragma once
#include "../../Core/Game.h"
#include "../../Core/Input/Keys.h"
#include "Inc/SimpleMath.h"
#include "Objects/Wall.h"

class Game;

class Pong
{
public:
    Pong(bool leftIsAI = false, bool rightIsAI = false)
        : leftIsAI(leftIsAI), rightIsAI(rightIsAI)
    {
        game = &Game::GetGame();
    }

    void Initialize();
    void Run();

    void AddWall(Game* game, WallType type);

    void AddRacket(Game* game, bool isAI,float speed, Vector3 center, Vector3 extends, Vector3 Color,
        Keys up = Keys::W, Keys down = Keys::S, Keys left = Keys::A, Keys right = Keys::D);
    void AddRacket(Game* game, bool isAI = false, float speed = 1.0);

    void AddBall(Game* game, Vector3 center = {0.f, 0.f, 0.f},
        Vector3 extends = {1.f, 1.f, 1.f},
        Vector3 Color = {1.f, 1.f, 1.f},
        float speed = 1.0);

    void IncreaseScoreByWall(WallType wall);
    Vector2 score = {0, 0};

    bool leftIsAI;
    bool rightIsAI;

protected:
    Game* game;
    //D3D_FEATURE_LEVEL featureLevel;
};
