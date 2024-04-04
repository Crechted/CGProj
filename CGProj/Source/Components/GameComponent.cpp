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
}
