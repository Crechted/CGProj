#include "Core/Engine.h"
#include "Windisplay.h"

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#include <d3d.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <chrono>

#include "Components/CollisionComponent.h"
#include "Core/Objects/Object.h"
#include "Components/GameComponent.h"
#include "Components/MeshComponent.h"
#include "Components/TextureComponent.h"
#include "Core/Input/InputDevice.h"
#include "Game/GameSquare.h"
#include "Game/Camera.h"
#include "Game/Components/LightComponents/DirectionalLightComponent.h"
#include "Objects/Mesh.h"
#include "Render/BlendState.h"
#include "Render/DeferredLightTechnique.h"
#include "Render/PostProcess.h"
#include "Render/RenderTarget.h"
#include "Render/Shader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


Engine::Engine()
{
}

Engine::~Engine()
{
    Destroy();
}

void Engine::Destroy()
{
    for (const auto gObj : gameObjects)
    {
        delete gObj;
    }

    for (const auto gComp : gameComponents)
    {
        delete gComp;
    }
    for (auto plData : pipelinesData)
    {
        plData->lastPostProc->Destroy();
        plData->swapChain->Release();
        plData->context->Release();
        plData->device->Release();

        for (const auto cam : GetCamerasOnViewport())
        {
            delete cam;
        }
    }

    for (const auto postProc : postProcesses)
    {
        delete postProc;
    }
    for (const auto postRend : postRenderObjects)
    {
        delete postRend;
    }
    for (const auto partSys : particleSystems)
    {
        delete partSys;
    }
    delete texRenderTarget;
    if (lightsBuffer) lightsBuffer->Destroy();
    if (lightsSRV) lightsSRV->Release();

    if (alphaBlendState) alphaBlendState->Destroy();
    if (deferredLight) deferredLight->Destroy();

    TextureComponent::Clear();
    Shader::Clear();
}

Engine& Engine::GetInstance()
{
    static Engine game;
    return game;
}

int32_t Engine::GetIdxCurrentPipeline()
{
    for (int32_t i = 0; i < pipelinesData.size(); i++)
    {
        if (pipelinesData[i] == curPlData) return i;
    }
    return -1;
}

Camera* Engine::CreateCamera(ViewType ViewType)
{
    const auto cam = CreateObject<Camera>();
    cam->viewType = ViewType;
    return cam;
}

void Engine::SetCurCamera(Camera* cam)
{
    if (cam)
    {
        curCam = cam;
    }
}

Camera* Engine::GetCurCamera() const
{
    return curCam ? curCam : cameras[0];
}

void Engine::SetRenderType(RenderType type)
{
    renderType = type;
}

RenderTarget* Engine::GetBackBufferRTV() const
{
    return curPlData->lastPostProc->GetRenderTarget();
}

void Engine::AddWindow(int32_t scrWidth, int32_t scrHeight, int32_t posX, int32_t posY, ViewType vType)
{
    if (cameras.isEmpty()) CreateCamera(vType);
    auto plData = new PipelineData();
    curPlData = plData;
    curPlData->display = new WinDisplay();
    curPlData->display->screenWidth = scrWidth;
    curPlData->display->screenHeight = scrHeight;
    curPlData->display->posX = posX;
    curPlData->display->posY = posY;
    pipelinesData.insert(plData);
}

void Engine::UpdateLightsData()
{
    lightsData.clear();
    for (const auto lightComp : lightComponents)
    {
        lightsData.insert(lightComp->GetLightData());
    }
}

void Engine::BindLightsBuffer()
{
    lightsBuffer->UpdateData(&lightsData[0]);
    GetContext()->PSSetShaderResources(8, 1, &lightsSRV);

    for (const auto light : lightComponents)
    {
        light->UpdateLightData();
        if (GetRenderState() == RenderState::Forward_Normal || GetRenderState() == RenderState::Deferred_Lighting)
            light->BindShadowMapSRV();
    }
}

void Engine::SetRenderState(RenderState state)
{
    renderState = state;
    OnChangeRenderStateDelegate.Broadcast(state);
}


void Engine::Initialize()
{
    if (pipelinesData.isEmpty()) AddWindow(850, 850);

    inputDevice = new InputDevice(&GetInstance());
    if (!GetDisplay() || !inputDevice) return;

    for (auto plData : pipelinesData)
    {
        curPlData = plData;
        curPlData->display->WindowInit();
        CreateDeviceAndSwapChain();
        CreateBlendState();
        CreateLightsBuffer();
        texRenderTarget = new RenderTarget(TargetViewType::Texture, GetDisplay()->screenWidth, GetDisplay()->screenHeight);
        texRenderTarget->CreateAll();
        //texRenderTarget->CreateDepthStencilView();
        //texRenderTarget->CreateDepthStencilState(false);
        curPlData->lastPostProc = new PostProcess(GetDisplay()->screenWidth, GetDisplay()->screenHeight);
        curPlData->lastPostProc->GetRenderTarget()->SetTargetType(TargetViewType::BackBuffer);
        curPlData->lastPostProc->Initialize();

        deferredLight = new DeferredLightTechnique();
        deferredLight->Initialize();

        for (const auto light : lightComponents)
        {
            light->Initialize();
        }
        for (const auto Comp : gameComponents)
        {
            Comp->Initialize();
        }
        for (const auto Obj : gameObjects)
        {
            Obj->Initialize();
        }
        for (const auto cam : GetCamerasOnViewport())
        {
            cam->Initialize();
        }
        for (const auto postProc : postProcesses)
        {
            postProc->Initialize();
        }
        for (const auto postRend : postRenderObjects)
        {
            postRend->Initialize();
        }
        for (const auto partSys : particleSystems)
        {
            partSys->Initialize();
        }
    }
}

void Engine::Run()
{
    PrevTime = std::chrono::steady_clock::now();

    bool isExitRequested = false;
    while (!isExitRequested)
    {
        Input(isExitRequested);
        for (auto plData : pipelinesData)
        {
            curPlData = plData;
            Update();
            Render();
        }
    }

    std::cout << "End!\n";
}

void Engine::Input(bool& isExitRequested)
{
    // Handle the windows messages.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        //TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (inputDevice->IsKeyDown(Keys::Escape))
    {
        isExitRequested = true;
    }
}

void Engine::Update()
{

    const float deltaTime = UpdateTime();

    for (const auto light : lightComponents)
    {
        light->Update(deltaTime);
    }
    for (const auto Comp : gameComponents)
    {
        Comp->Update(deltaTime);
    }

    for (const auto Obj : gameObjects)
    {
        Obj->Update(deltaTime);
    }
    for (const auto cam : GetCamerasOnViewport())
    {
        cam->Update(deltaTime);
    }
    for (const auto postProc : postProcesses)
    {
        postProc->Update(deltaTime);
    }
    for (const auto postRend : postRenderObjects)
    {
        postRend->Update(deltaTime);
    }
    for (const auto partSys : particleSystems)
    {
        partSys->Update(deltaTime);
    }

    DetectOverlapped();
}

float Engine::UpdateTime()
{
    curTime = std::chrono::steady_clock::now();
    const float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
    PrevTime = curTime;

    totalTime += deltaTime;
    frameCount++;

    /*if (totalTime > 1.0f)
    {
        float fps = frameCount / totalTime;

        totalTime -= 1.0f;

        WCHAR text[256];
        swprintf_s(text, TEXT("FPS: %f"), fps);
        SetWindowText(GetDisplay()->hWnd, text);

        frameCount = 0;
    }*/

    float fps = frameCount / totalTime;

    //totalTime -= 1.0f;

    WCHAR text[256];
    swprintf_s(text, TEXT("FPS: %f"), fps);
    SetWindowText(GetDisplay()->hWnd, text);

    //frameCount = 0;
    return deltaTime;
}

void Engine::DetectOverlapped()
{
    Array<CollisionComponent*> collisions;
    for (const auto obj : gameObjects)
    {
        if (const auto mesh = dynamic_cast<Mesh*>(obj))
        {
            const auto collision = mesh->GetCollision();
            if (collision) collisions.insert(collision);
        }
        else
        {
            for (const auto comp : obj->gameComponents)
            {
                if (const auto meshComp = dynamic_cast<MeshComponent*>(comp))
                {
                    const auto collision = meshComp->GetMeshCollision();
                    if (collision) collisions.insert(collision);
                }
            }
        }
    }

    for (const auto comp : gameComponents)
    {
        if (const auto meshComp = dynamic_cast<MeshComponent*>(comp))
        {
            const auto collision = meshComp->GetMeshCollision();
            if (collision) collisions.insert(collision);
        }

    }

    for (int32_t i = 0; i < collisions.size(); i++)
    {
        const auto col1 = collisions[i];
        for (int32_t j = i + 1; j < collisions.size(); j++)
        {
            const auto col2 = collisions[j];

            if (col1->Contains(col2) && !col1->GetOverlappedCollisions().contains(col2))
            {
                col1->beginOverlapped.Broadcast(col2);
                col2->beginOverlapped.Broadcast(col1);
            }
            else if (!col1->Contains(col2) && col1->GetOverlappedCollisions().contains(col2))
            {
                col1->endOverlapped.Broadcast(col2);
                col2->endOverlapped.Broadcast(col1);
            }
        }
    }
}


void Engine::Render()
{
    GetContext()->ClearState();
    UpdateLightsData();
    SortMeshesByTransparency();

    for (const auto light : lightComponents)
    {
        if (light->GetLightData().type != LightType::DirectionalLight) continue;
        SetRenderState(useCascadeShadow ? RenderState::CascadeShadow : RenderState::ShadowMap);
        SetCurEyeData(light->GetEyeData());
        light->SetDepthStencil();
        light->ClearDepthStencil();

        for (uint32_t i = 0; i < (/*useCascadeShadow ? CASCADE_COUNT :*/ 1); i++)
        {
            if (useCascadeShadow) light->SetCurrentCascadeData(i);
            RenderScene();
        }
    }

    SetCurEyeData(GetCurCamera()->GetEyeData());
    switch (renderType)
    {
        case RenderType::Forward:
        {
            ForwardRender();
            break;
        }
        case RenderType::Deferred:
        {
            DeferredRender();
            break;
        }
    }
    texRenderTarget->BindTarget();
    SetRenderState(RenderState::DrawDebug);
    RenderScene(true, false);

    SetRenderState(RenderState::Forward_Transparent);
    RenderScene(false, true);

    SetRenderState(RenderState::PostProcess);
    auto prevResourceView = texRenderTarget->GetRenderTargetSRV();
    for (int32_t i = 0; i < postProcesses.size(); i++)
    {
        postProcesses[i]->SetSRV(prevResourceView);
        postProcesses[i]->Draw();
        prevResourceView = postProcesses[i]->GetRenderTarget()->GetRenderTargetSRV();
    }

    curPlData->lastPostProc->SetSRV(prevResourceView);
    curPlData->lastPostProc->Draw();

    SetRenderState(RenderState::PostRender);
    for (const auto postRend : postRenderObjects)
    {
        postRend->Draw();
    }

    curPlData->swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/0);
}

void Engine::SortMeshesByTransparency()
{
    opaqueObjects.clear();
    transparentObjects.clear();
    opaqueComponents.clear();
    transparentComponents.clear();

    for (const auto obj : gameObjects)
    {
        if (const auto mesh = dynamic_cast<Mesh*>(obj))
        {
            if (mesh->GetOpacity() < 1.0f)
            {
                InsertNewTransparentMesh(mesh);
                continue;
            }
        }
        opaqueObjects.insert(obj);
    }

    for (const auto comp : gameComponents)
    {
        if (const auto mesh = dynamic_cast<MeshComponent*>(comp))
        {
            if (mesh->GetMesh()->GetOpacity() < 1.0f)
            {
                InsertNewTransparentMeshComponent(mesh);
                continue;
            }
        }
        opaqueComponents.insert(comp);
    }
}

void Engine::InsertNewTransparentMesh(Mesh* const mesh)
{
    const auto camScene = GetCurCamera()->GetSceneComponent();
    if (transparentObjects.isEmpty()) transparentObjects.insert(mesh);
    else
        for (int32_t i = 0; i < transparentObjects.size(); ++i)
        {
            const float meshDistanceToCam = mesh->GetSceneComponent()->GetDistanceTo(camScene);
            const float transDistanceToCam = transparentObjects[i]->GetSceneComponent()->GetDistanceTo(camScene);
            if (meshDistanceToCam > transDistanceToCam)
            {
                transparentObjects.insert(i, mesh);
                return;
            }
        }
    transparentObjects.insert(mesh);
}

void Engine::InsertNewTransparentMeshComponent(MeshComponent* const mesh)
{
    const auto camScene = GetCurCamera()->GetSceneComponent();
    if (transparentComponents.isEmpty()) transparentComponents.insert(mesh);
    else
        for (int32_t i = 0; i < transparentComponents.size(); ++i)
        {
            const float meshDistanceToCam = mesh->GetMesh()->GetSceneComponent()->GetDistanceTo(camScene);
            const float transDistanceToCam = transparentComponents[i]->GetMesh()->GetSceneComponent()->GetDistanceTo(camScene);
            if (meshDistanceToCam > transDistanceToCam)
            {
                transparentComponents.insert(i, mesh);
                return;
            }
        }
    transparentComponents.insert(mesh);
}

void Engine::ForwardRender()
{
    texRenderTarget->BindTarget();
    texRenderTarget->Clear();
    SetRenderState(RenderState::Forward_Normal);
    RenderScene();
}

void Engine::DeferredRender()
{
    deferredLight->Render();
}

void Engine::CreateBlendState()
{
    alphaBlendState = (new BlendState())->CreateAlphaBlend();
}

void Engine::RenderScene(bool renderOpaque, bool renderTransparent)
{

    for (const auto cam : GetCamerasOnViewport())
    {
        cam->Render();
    }

    for (const auto light : lightComponents)
    {
        light->Render();
    }

    if (renderOpaque)
    {
        for (const auto Comp : opaqueComponents)
        {
            BindLightsBuffer();
            Comp->Render();
        }
        for (const auto Obj : opaqueObjects)
        {
            BindLightsBuffer();
            Obj->Render();
        }
    }

    if (renderTransparent)
    {
        alphaBlendState->Bind();
        for (const auto Comp : transparentComponents)
        {
            BindLightsBuffer();
            Comp->Render();
        }
        for (const auto Obj : transparentObjects)
        {
            BindLightsBuffer();
            Obj->Render();
        }
        alphaBlendState->UnBind();
    }

    if ( /*renderState == RenderState::Forward_Normal || renderState == RenderState::Deferred_GBuffer ||*/
        renderState == RenderState::Forward_Transparent)
        for (const auto partSys : particleSystems)
        {
            /*if (renderOpaque && partSys->GetTextureComponent()->GetOpacity() >= 1.0f)
                partSys->Render();
            else if (renderTransparent && partSys->GetTextureComponent()->GetOpacity() < 1.0f)*/
            partSys->Render();
        }
}

void Engine::CreateDeviceAndSwapChain()
{

    DXGI_SWAP_CHAIN_DESC swapDesc;
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = GetDisplay()->screenWidth;
    swapDesc.BufferDesc.Height = GetDisplay()->screenHeight;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = GetDisplay()->hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;                          // DXGI_SWAP_EFFECT_FLIP_DISCARD
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_RESTRICTED_TO_ALL_HOLOGRAPHIC_DISPLAYS; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};
    auto res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, D3D11_CREATE_DEVICE_DEBUG,
        featureLevel, 1,
        D3D11_SDK_VERSION, &swapDesc,
        &(curPlData->swapChain), &(curPlData->device), nullptr,
        &(curPlData->context));

    if (FAILED(res))
    {
        // Well, that was unexpected
    }
}

void Engine::CreateLightsBuffer()
{
    HRESULT hr;
    lightsBuffer = (new Buffer())->CreateBuffer<LightData>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(LightData) * lightComponents.size(), nullptr, sizeof(LightData));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = lightComponents.size();
    hr = GetDevice()->CreateShaderResourceView(lightsBuffer->GetBuffer(), &srvDesc, &lightsSRV);
    if (FAILED(hr)) return;
}
