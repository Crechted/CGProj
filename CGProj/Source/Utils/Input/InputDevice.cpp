#include "InputDevice.h"

#include "../../Core/Game.h"

InputDevice::InputDevice()
{
    game = &Game::GetGame();
}

InputDevice::~InputDevice()
{
}
