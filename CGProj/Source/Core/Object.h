#pragma once
#include "../Utils/Delegates.h"
#include "../Utils/Array/Array.h"
#include "../Components/MovementComponent.h"

class GameComponent;
class Game;

class Object
{
public:
    Object();
    virtual ~Object()
    {
        Object::Destroy();
    }

    virtual void Initialize();

    virtual void Update(float timeTick);

    template<typename T>
    T* CreateComponent()
    {
        if (const auto newComp = reinterpret_cast<GameComponent*>(new T()))
        {
            newComp->Owner = this;            
            gameComponents.insert(newComp);
            return reinterpret_cast<T*>(newComp);
        }
         return nullptr;  
    }

    virtual void Draw();

    virtual void Destroy()
    {
    }

    Array<GameComponent*> gameComponents;
    //Array<Object*> overlappedObjects;
    MulticastDelegate<Object*> beginOverlapped; 
    MulticastDelegate<Object*> endOverlapped; 
    Game* game;
};
