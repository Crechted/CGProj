#include "GameComponent.h"
#include "Core/Engine.h"

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

void GameComponent::PreDraw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->PreDraw();
    }
}

void GameComponent::Draw()
{
    for (auto gameComp : gameComponents)
    {
        gameComp->Draw();
    }
}

void GameComponent::Render()
{
    PreDraw();
    Draw();
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
