#pragma once
#include "../Utils/Delegates.h"
#include "../Utils/Array/Array.h"
#include "Components/GameComponent.h"

class GameComponent;
class Engine;

class Object
{
public:
    Object();
    virtual ~Object()
    {
        Object::Destroy();
    }

    virtual void Reload();
    virtual void Initialize();

    virtual void Update(float timeTick);

    template<typename T>
    T* CreateComponent()
    {
        if (const auto newComp = dynamic_cast<GameComponent*>(new T()))
        {
            newComp->Owner = this;            
            gameComponents.insert(newComp);
            return dynamic_cast<T*>(newComp);
        }
         return nullptr;  
    }

    Array<GameComponent*> gameComponents;
    virtual void Draw();

    virtual void Destroy();

    //Array<Object*> overlappedObjects;
    MulticastDelegate<Object*> beginOverlapped; 
    MulticastDelegate<Object*> endOverlapped; 
    Engine* engInst;
};
