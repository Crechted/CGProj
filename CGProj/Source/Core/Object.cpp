#include "Object.h"

#include "Game.h"
#include "../Components/GameComponent.h"

Object::Object()
{
    game = &Game::GetGame();
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