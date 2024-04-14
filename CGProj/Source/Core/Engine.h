#pragma once
#include <d3d11.h>
#include "Utils/Array/Array.h"
#include "Core/Objects/Object.h"
#include "Game/Camera.h"

enum class ViewType : uint8_t;
class Camera;
class Object;
class InputDevice;
class WinDisplay;

struct PipelineData
{
    WinDisplay* display;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapChain;
    DXGI_SWAP_CHAIN_DESC swapDesc;

    ID3D11RenderTargetView* renderTargetView;

    Array<D3D11_VIEWPORT> viewports;
    Array<Camera*> cameras;
    uint32_t viewportsNum = 1;
    uint32_t curViewport = 0;

    ID3D11Texture2D* depthStencil;
    ID3D11DepthStencilView* depthStencilView;
};

class Engine
{

public:
    Engine(const Engine& obj) = delete;
    virtual ~Engine();
    void Destroy();
    static Engine& GetInstance();

    virtual void Run();
    virtual void Update();
    virtual void Render();
    virtual void Initialize();
    void Input(bool& isExitRequested);
    void DetectOverlapped();

    float GetTotalTime() const { return totalTime; }
    Camera* CreateCamera(ViewType ViewType = ViewType::Perspective);
    void AddCamera(Camera* otherCam);

    ID3D11DeviceContext* GetContext() const { return curPlData->context; }
    ID3D11Device* GetDevice() const { return curPlData->device; }
    WinDisplay* GetDisplay() const { return curPlData->display; }
    Camera* GetCurCamera() const { return curPlData->cameras[curPlData->curViewport]; }
    Array<Camera*>& GetCamerasOnViewport() const { return curPlData->cameras; }
    InputDevice* GetInputDevice() const { return inputDevice; }
    int32_t GetIdxCurrentPipeline();


    //PipelineData plData;

    void AddWindow(int32_t scrWidth, int32_t scrHeigh, int32_t posX, int32_t posY,  const Array<Camera*>& cameras);
    void AddWindow(int32_t scrWidth = -1, int32_t scrHeight = -1, int32_t posX = -1, int32_t posY = -1, ViewType = ViewType::General);

    template <typename T, typename... Args>
    T* CreateObject(const Args&... args)
    {
        T* newObj = new T(args...);
        if (const auto nGObj = dynamic_cast<Object*>(newObj))
        {
            gameObjects.insert(nGObj);
            return newObj;
        }
        delete newObj;
        return nullptr;

    }

    template <typename T, typename... Args>
    T* CreateComponent(const Args&... args)
    {
        T* newComp = new T(args...);
        if (const auto nComp = dynamic_cast<GameComponent*>(newComp))
        {
            gameComponents.insert(nComp);
            nComp->Owner = nullptr;
            return newComp;
        }
        return nullptr;
    }

protected:
    //D3D_FEATURE_LEVEL featureLevel;    
    InputDevice* inputDevice;
    PipelineData* curPlData;
    explicit Engine();

    Array<Object*> gameObjects;
    Array<Camera*> cameras;
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
