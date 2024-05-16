#pragma once
#include <d3d11.h>
#include "Utils/Array/Array.h"
#include "Core/Objects/Object.h"
#include "Game/Camera.h"
#include "Game/Components/LightComponents/LightComponent.h"
#include "Render/PostProcess.h"

class Mesh;
class MeshComponent;
class DeferredLightTechnique;
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

    PostProcess* lastPostProc = nullptr;
};

enum class RenderState
{
    ShadowMap,
    CascadeShadow,
    Forward_Normal,
    Forward_Transparent,
    Deferred_GBuffer,
    Deferred_Lighting,
    DrawDebug,
    PostProcess
};

enum class RenderType
{
    Forward,
    Deferred
};

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2
static constexpr uint32_t CASCADE_COUNT = 8;

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
    void RenderScene(bool renderOpaque = true, bool renderTransparent = false);
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

    void SetCurCamera(Camera* cam);
    Camera* GetCurCamera() const;

    void SetRenderState(RenderState state);
    RenderState GetRenderState() const { return renderState; }

    void SetRenderType(RenderType type);
    RenderType GetRenderType() const { return renderType; }

    EyeViewData GetCurEyeData() const { return curEyeData; }

    Array<Camera*> GetCamerasOnViewport() const { return cameras; }
    Array<Object*> GetGameObjects() const { return gameObjects; }
    Array<GameComponent*> GetGameComponents() const { return gameComponents; }
    Array<LightComponent*> GetLightComponents() const { return lightComponents; }

    Array<LightComponent*>& GetLightComponents() { return lightComponents; }

    InputDevice* GetInputDevice() const { return inputDevice; }
    int32_t GetIdxCurrentPipeline();

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

    void UpdateLightsData();
    void BindLightsBuffer();
    RenderTarget* GetTexRenderTarget() const { return texRenderTarget; }
    MulticastDelegate<RenderState> OnChangeRenderStateDelegate;
    bool useCascadeShadow = false;

protected:
    //D3D_FEATURE_LEVEL featureLevel;
    RenderType renderType = RenderType::Forward;
    RenderState renderState;
    InputDevice* inputDevice;
    PipelineData* curPlData;
    DeferredLightTechnique* deferredLight = nullptr;

    ID3D11Buffer* lightsBuffer;
    ID3D11ShaderResourceView* lightsSRV;
    
    ID3D11BlendState* blendState = nullptr;

    explicit Engine();

    Array<Object*> gameObjects;
    Array<Object*> opaqueObjects;
    Array<Mesh*> transparentObjects;
    Array<GameComponent*> gameComponents;
    Array<GameComponent*> opaqueComponents;
    Array<MeshComponent*> transparentComponents;
    Array<Camera*> cameras;
    Array<PostProcess*> postProcesses;
    Array<LightComponent*> lightComponents;
    Array<PipelineData*> pipelinesData;
    Array<LightData> lightsData;
    Camera* curCam;

    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::chrono::time_point<std::chrono::steady_clock> curTime;
    float totalTime = 0;
    unsigned int frameCount = 0;

    MSG msg;

    void SortMeshesByTransparency();
private:
    EyeViewData curEyeData;

    void InsertNewTransparentMesh(Mesh* mesh);
    void InsertNewTransparentMeshComponent(MeshComponent* mesh);
    virtual void ForwardRender();
    virtual void DeferredRender();

    
    void CreateBlendState();
    void CreateDeviceAndSwapChain();
    void CreateLightsBuffer();

    RenderTarget* texRenderTarget;
};
