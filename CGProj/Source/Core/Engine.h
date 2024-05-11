#pragma once
#include <d3d11.h>
#include "Utils/Array/Array.h"
#include "Core/Objects/Object.h"
#include "Game/Camera.h"
#include "Game/Components/LightComponents/LightComponent.h"
#include "Render/PostProcess.h"

class PostProcess;
class LightComponent;
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
    int32_t viewportsNum = 1;
    int32_t curViewport = 0;

    ID3D11Texture2D* depthStencil;
    ID3D11DepthStencilView* depthStencilView;
};

enum class RenderState
{
    ShadowMap,
    CascadeShadow,
    Normal,
    PostProcess
};

static constexpr  uint32_t CASCADE_COUNT = 8;

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
    float UpdateTime();
    void DetectOverlapped();

    float GetTotalTime() const { return totalTime; }
    Camera* CreateCamera(ViewType ViewType = ViewType::Perspective);

    ID3D11DeviceContext* GetContext() const { return curPlData->context; }
    ID3D11Device* GetDevice() const { return curPlData->device; }
    WinDisplay* GetDisplay() const { return curPlData->display; }
    IDXGISwapChain* GetSwapChain() const { return curPlData->swapChain; }

    void SetCurCamera(Camera* cam) { curCam = cam; }

    Camera* GetCurCamera() const
    {
        return curCam ? curCam : curPlData->cameras[curPlData->curViewport];
    }

    EyeViewData GetCurEyeData() const { return curEyeData; }

    Array<Camera*>& GetCamerasOnViewport() const
    {
        return curPlData->cameras;
    }

    Array<LightComponent*>& GetLightComponents()
    {
        return lightComponents;
    }

    InputDevice* GetInputDevice() const { return inputDevice; }
    int32_t GetIdxCurrentPipeline();


    //PipelineData plData;

    void AddWindow(int32_t scrWidth = -1, int32_t scrHeight = -1, int32_t posX = -1, int32_t posY = -1, ViewType = ViewType::General);

    template <typename T, typename... Args>
    T* CreateObject(const Args&... args)
    {
        T* newObj = new T(args...);
        if (const auto nGObj = dynamic_cast<Object*>(newObj))
        {
            if (const auto nCam = dynamic_cast<Camera*>(newObj)) cameras.insert(nCam);
            else if (const auto nPostProc = dynamic_cast<PostProcess*>(newObj)) postProcesses.insert(nPostProc);
            else gameObjects.insert(nGObj);

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
            if (const auto nLight = dynamic_cast<LightComponent*>(nComp)) lightComponents.insert(nLight);
            else gameComponents.insert(nComp);

            nComp->Owner = nullptr;
            return newComp;
        }
        return nullptr;
    }

    RenderState GetCurrentRenderState() const { return renderState; }
    MulticastDelegate<RenderState> OnChangeRenderStateDelegate;

    bool useCascadeShadow = false;
protected:
    //D3D_FEATURE_LEVEL featureLevel;
    RenderState renderState;
    InputDevice* inputDevice;
    PipelineData* curPlData;


    explicit Engine();

    Array<Object*> gameObjects;
    Array<Camera*> cameras;
    Array<PostProcess*> postProcesses;
    Array<LightComponent*> lightComponents;
    Array<GameComponent*> gameComponents;
    Array<PipelineData*> pipelinesData;
    Camera* curCam;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::chrono::time_point<std::chrono::steady_clock> curTime;
    float totalTime = 0;
    unsigned int frameCount = 0;

    MSG msg;
private:
    EyeViewData curEyeData;
    void CreateDeviceAndSwapChain();
    void CreateTargetViewAndViewport();
    void CreateDepthStencilView();
    void RenderScene();

    RenderTarget* texRenderTarget;
    void OnChangeRenderState(RenderState state);
};
