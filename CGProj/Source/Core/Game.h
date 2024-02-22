#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "../Utils/Array/Array.h"

class GameComponent;
class WinDisplay;

class Game
{

public:
    Game(const Game& obj) = delete;
    ~Game();
    static Game& GetGame();

    virtual void Run();
    virtual void Update();
    virtual void Render();
    virtual void Initialize();
    void Input(bool& isExitRequested);

    float GetTotalTime() const {return totalTime;}
    WinDisplay* display;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    DXGI_SWAP_CHAIN_DESC swapDesc;

    Array<GameComponent*> gameComponents;

private:
    //D3D_FEATURE_LEVEL featureLevel;
    
    explicit Game()
    {
    }

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::chrono::time_point<std::chrono::steady_clock> curTime;
    float totalTime = 0;
    unsigned int frameCount = 0;

    MSG msg;
};
