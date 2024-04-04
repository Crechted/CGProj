#pragma once
#include "../../Core/Engine.h"
#include "../../Core/Input/Keys.h"
#include "SimpleMath.h"
#include "Objects/Wall.h"

class Engine;

class Pong
{
public:
    Pong(bool leftIsAI = false, bool rightIsAI = false)
        : leftIsAI(leftIsAI), rightIsAI(rightIsAI)
    {
        engInst = &Engine::GetInstance();
    }

    void Initialize();
    void Run();

    void AddWall(Engine* engInst, WallType type);
    
    void AddWall(Engine* engInst, WallType type, Vector3 center,
       Vector3 extends = {0.97f, 0.02f, 0.5f},
       Vector3 Color = {1.f, 1.f, 1.f});


    void AddRacket(Engine* engInst, bool isAI,float speed, Vector3 center, Vector3 extends, Vector3 Color,
        Keys up = Keys::W, Keys down = Keys::S, Keys left = Keys::A, Keys right = Keys::D);
    void AddRacket(Engine* engInst, bool isAI = false, float speed = 1.0);

    void AddBall(Engine* engInst, Vector3 center = {0.f, 0.f, 0.f},
        Vector3 extends = {1.f, 1.f, 1.f},
        Vector3 Color = {1.f, 1.f, 1.f},
        float speed = 1.0);

    void IncreaseScoreByWall(WallType wall);
    Vector2 score = {0, 0};

    bool leftIsAI;
    bool rightIsAI;

protected:
    Engine* engInst;
    //D3D_FEATURE_LEVEL featureLevel;
};
