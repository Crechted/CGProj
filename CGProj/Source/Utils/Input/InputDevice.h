#pragma once
class Game;

class InputDevice
{
    friend class Game;

    Game* game;

public:
    InputDevice();
    ~InputDevice();
};
