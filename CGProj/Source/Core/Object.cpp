#include "Object.h"

#include "Game.h"

Object::Object()
{
    game = &Game::GetGame();
}
