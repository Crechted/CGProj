#include "Object.h"

#include "Engine.h"
#include "../Components/GameComponent.h"

Object::Object()
{
    engInst = &Engine::GetInstance();
}

void Object::Reload()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Reload();
    }
}

void Object::Initialize()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Initialize();
    }
}

void Object::Update(float timeTick)
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Update(timeTick);
    }
}

void Object::Draw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Draw();
    }
}

void Object::Destroy()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->DestroyResource();
    }
}
