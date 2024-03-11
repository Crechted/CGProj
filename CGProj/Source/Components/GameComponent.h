#pragma once

class Object;
class Game;

class GameComponent
{
public:
    GameComponent();

    virtual ~GameComponent();

    virtual void Initialize() = 0;
    virtual void DestroyResource();
    virtual void Draw() {}
    virtual void Reload(){}    
    virtual void Update(float timeTick) = 0;

    Object* Owner;
    Game* game;
};
