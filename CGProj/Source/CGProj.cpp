#include "Game/Pong/Pong.h"

int main()
{
    Pong* pong = new Pong();
    pong->Initialize();
    pong->Run();

    return 0;
}


/*      DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),*/
