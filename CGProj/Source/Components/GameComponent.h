#pragma once

class Game;

class GameComponent
{
public:
    GameComponent();

    virtual ~GameComponent();

    virtual void Initialize() = 0;
    virtual void DestroyResource();
    virtual void Draw() = 0;
    virtual void Reload() = 0;
    virtual void Update(float timeTick) = 0;

    Game* game;
};
