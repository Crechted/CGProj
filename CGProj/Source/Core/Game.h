#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "../Utils/Array/Array.h"

#include "../Core/Object.h"
#include "../Game/Camera.h"

enum class ViewType : uint8_t;
class Camera;
class Object;
class InputDevice;
class WinDisplay;

struct PipelineData
{
    WinDisplay* display;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    DXGI_SWAP_CHAIN_DESC swapDesc;

    ID3D11RenderTargetView* renderTargetView;
    D3D11_VIEWPORT viewport;

    ID3D11Texture2D* depthStencil;
    ID3D11DepthStencilView* depthStencilView;
    Camera* camera;
};

class Game
{

public:
    Game(const Game& obj) = delete;
    virtual ~Game();
    void Destroy();
    static Game& GetGame();

    virtual void Run();
    virtual void Update();
    virtual void Render();
    virtual void Initialize();
    void Input(bool& isExitRequested);
    void DetectOverlapped();

    float GetTotalTime() const { return totalTime; }
    Camera* CreateCamera(ViewType ViewType = ViewType::Perspective);

    ID3D11DeviceContext* GetContext() const { return curPlData->context; }
    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const { return curPlData->device; }
    WinDisplay* GetDisplay() const { return curPlData->display; }
    Camera* GetCamera() const { return curPlData->camera; }
    InputDevice* GetInputDevice() const { return inputDevice; }
    int32_t GetIdxCurrentPipeline();


    //PipelineData plData;

    void AddWindow(int32_t scrWidth, int32_t scrHeigh, int32_t posX, int32_t posY, Camera* camera);
    void AddWindow(int32_t scrWidth = -1, int32_t scrHeight = -1, int32_t posX = -1, int32_t posY = -1, ViewType = ViewType::General);

    template <typename T>
    T* CreateObject()
    {
        T* newObj = new T();
        if (const auto nGObj = reinterpret_cast<Object*>(newObj))
        {
            gameObjects.insert(nGObj);
            return newObj;
        }
        return nullptr;

    }

    template <typename T>
    T* CreateComponent()
    {
        if (const auto nComp = reinterpret_cast<GameComponent*>(new T()))
        {
            gameComponents.insert(nComp);
            nComp->Owner = nullptr;
            return reinterpret_cast<T*>(nComp);
        }
        return nullptr;
    }

protected:
    //D3D_FEATURE_LEVEL featureLevel;    
    InputDevice* inputDevice;
    PipelineData* curPlData;
    explicit Game();

    Array<Object*> gameObjects;
    Array<GameComponent*> gameComponents;
    Array<PipelineData*> pipelinesData;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::chrono::time_point<std::chrono::steady_clock> curTime;
    float totalTime = 0;
    unsigned int frameCount = 0;

    MSG msg;

    void CreateDeviceAndSwapChain();
    void CreateTargetViewAndViewport();
    void CreateDepthStencilView();
};
