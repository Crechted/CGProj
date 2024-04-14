#pragma once
#include "Utils/Delegates.h"
#include "Utils/Array/Array.h"
#include "Core/Components/GameComponent.h"

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

    virtual void PreDraw();
    virtual void Draw();
    virtual void Render();
    virtual void Destroy();
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

    //Array<Object*> overlappedObjects;
    Engine* engInst;
};
