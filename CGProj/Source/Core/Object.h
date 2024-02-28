#pragma once
#include "../Utils/Delegates.h"
#include "../Utils/Array/Array.h"

class Game;

class Object
{
public:
    Object();
    virtual ~Object()
    {
        Object::Destroy();
    }

    virtual void Initialize()
    {
    }

    virtual void Update(float timeTick)
    {
    }

    virtual void Draw()
    {
    }

    virtual void Destroy()
    {
    }

    //Array<Object*> overlappedObjects;
    MulticastDelegate<Object*> beginOverlapped; 
    MulticastDelegate<Object*> endOverlapped; 
    Game* game;
};
