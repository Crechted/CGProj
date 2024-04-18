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

    template <typename T, typename... Args>
    T* CreateComponent(const Args&... args)
    {
        if (const auto newComp = dynamic_cast<GameComponent*>(new T(args...)))
        {
            newComp->Owner = this;
            gameComponents.insert(newComp);
            return dynamic_cast<T*>(newComp);
        }
        return nullptr;
    }

    GameComponent* outer = nullptr;
    Array<GameComponent*> gameComponents;

    //Array<Object*> overlappedObjects;
    Engine* engInst;
};
