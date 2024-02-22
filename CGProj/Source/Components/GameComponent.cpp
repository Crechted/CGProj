#include "GameComponent.h"
#include "../Core/Game.h"

GameComponent::GameComponent()
{
    game = &Game::GetGame();
}

GameComponent::~GameComponent()
{
    DestroyResource();
}

void GameComponent::DestroyResource()
{
}
