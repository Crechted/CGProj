#pragma once
#include "Utils/Array/Array.h"

class Object;
class Engine;

class GameComponent
{
public:
    GameComponent();

    virtual ~GameComponent();

    virtual void Initialize();
    virtual void DestroyResource();
    virtual void PreDraw();
    virtual void Draw();
    virtual void Render();
    virtual void Reload();    
    virtual void Update(float timeTick);

    template<typename T, typename... Args>
    T* CreateComponent(const Args&... args)
    {
        if (const auto newComp = dynamic_cast<GameComponent*>(new T(args...)))
        {
            newComp->Owner = Owner;            
            gameComponents.insert(newComp);
            return dynamic_cast<T*>(newComp);
        }
        return nullptr;  
    }

    Array<GameComponent*> gameComponents;
    
    Object* Owner;
    Engine* engInst;
};
