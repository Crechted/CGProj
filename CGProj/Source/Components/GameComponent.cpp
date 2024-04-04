#include "GameComponent.h"
#include "..\Core\Engine.h"

GameComponent::GameComponent()
{
    engInst = &Engine::GetInstance();
}

GameComponent::~GameComponent()
{
    DestroyResource();
}

void GameComponent::DestroyResource()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->DestroyResource();
    }
}

void GameComponent::Reload()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Reload();
    }
}

void GameComponent::Initialize()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Initialize();
    }
}

void GameComponent::Update(float timeTick)
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Update(timeTick);
    }
}

void GameComponent::Draw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Draw();
    }
}
