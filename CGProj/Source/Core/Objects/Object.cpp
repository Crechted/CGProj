#include "Object.h"

#include "Core/Engine.h"
#include "Core/Components/GameComponent.h"

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

void Object::PreDraw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->PreDraw();
    }
}

void Object::Draw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Draw();
    }
}

void Object::Render()
{
    PreDraw();
    Draw();
}

void Object::Update(float timeTick)
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Update(timeTick);
    }
}

void Object::Destroy()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->DestroyResource();
    }
}
