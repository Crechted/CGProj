#include "Engine.h"
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
        plData->renderTargetView->Release();
        plData->depthStencil->Release();
        plData->depthStencilView->Release();
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
    delete texRenderTarget;
    if (lightsBuffer) lightsBuffer->Release();
    if (lightsSRV) lightsSRV->Release();
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
    curPlData->cameras = cameras;
    curPlData->viewportsNum = cameras.size();
    pipelinesData.insert(plData);
}

void Engine::UpdateLightsBuffer()
{
    Array<LightData> lightsData;
    for (const auto lightComp : lightComponents)
    {
        lightsData.insert(lightComp->GetLightData());
    }
    GetContext()->UpdateSubresource(lightsBuffer, 0, nullptr, &lightsData[0], 0, 0);
    GetContext()->PSSetShaderResources(8, 1, &lightsSRV);
}


void Engine::Initialize()
{
    if (pipelinesData.isEmpty()) AddWindow();

    inputDevice = new InputDevice(&GetInstance());
    if (!GetDisplay() || !inputDevice) return;

    OnChangeRenderStateDelegate.AddRaw(this, &Engine::OnChangeRenderState);
    for (auto plData : pipelinesData)
    {
        curPlData = plData;
        curPlData->display->WindowInit();
        CreateDeviceAndSwapChain();
        CreateTargetViewAndViewport();
        CreateDepthStencilView();
        CreateLightsBuffer();
        const auto postPro = CreateObject<PostProcess>(GetDisplay()->screenWidth, GetDisplay()->screenHeight);
        postPro->GetRenderTarget()->SetTargetType(TargetViewType::BackBuffer);
        texRenderTarget = new RenderTarget(TargetViewType::Texture, GetDisplay()->screenWidth, GetDisplay()->screenHeight);
        texRenderTarget->Initialize();
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
    float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    curPlData->context->ClearState();

    for (const auto light : lightComponents)
    {
        if (light->GetLightData().type != 2) continue;
        OnChangeRenderStateDelegate.Broadcast(useCascadeShadow ? RenderState::CascadeShadow : RenderState::ShadowMap);
        curEyeData = light->GetEyeData();
        curEyeData.isCam = false;
        light->SetDepthStencil();
        light->ClearDepthStencil();

        for (uint32_t i = 0; i < (useCascadeShadow ? CASCADE_COUNT : 1); i++)
        {
            if (useCascadeShadow) light->SetCurrentCascadeData(i);
            RenderScene();
        }
    }

    const auto texTargetView = texRenderTarget->GetRenderTargetView();
    GetContext()->OMSetRenderTargets(1, &texTargetView, curPlData->depthStencilView);
    GetContext()->ClearRenderTargetView(texTargetView, color);
    OnChangeRenderStateDelegate.Broadcast(RenderState::Normal);
    for (int32_t i = 0; i < curPlData->viewportsNum; i++)
    {
        curPlData->curViewport = i;
        curEyeData.isCam = true;
        GetContext()->RSSetViewports(1, &curPlData->viewports[i]);
        RenderScene();
    }

    OnChangeRenderStateDelegate.Broadcast(RenderState::PostProcess);
    auto prevResourceView = texRenderTarget->GetShaderResourceView();
    for (int32_t i = 0; i < postProcesses.size() - 1; i++)
    {
        postProcesses[i]->SetSRV(prevResourceView);
        postProcesses[i]->Draw();
        prevResourceView = postProcesses[i]->GetRenderTarget()->GetShaderResourceView();
    }

    GetContext()->ClearDepthStencilView(curPlData->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    postProcesses[postProcesses.size() - 1]->SetSRV(prevResourceView);
    postProcesses[postProcesses.size() - 1]->Draw();

    curPlData->swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);

}

void Engine::RenderScene()
{
    for (const auto light : lightComponents)
    {
        light->Render();
    }
    for (const auto Comp : gameComponents)
    {
        Comp->Render();
    }
    for (const auto Obj : gameObjects)
    {
        Obj->Render();
    }
    for (const auto cam : GetCamerasOnViewport())
    {
        curEyeData = cam->GetEyeData();
        cam->Render();
    }
}

void Engine::OnChangeRenderState(RenderState state)
{
    renderState = state;
}

void Engine::CreateDeviceAndSwapChain()
{
    curPlData->swapDesc.BufferCount = 2;
    curPlData->swapDesc.BufferDesc.Width = GetDisplay()->screenWidth;
    curPlData->swapDesc.BufferDesc.Height = GetDisplay()->screenHeight;
    curPlData->swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    curPlData->swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    curPlData->swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    curPlData->swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    curPlData->swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    curPlData->swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    curPlData->swapDesc.OutputWindow = GetDisplay()->hWnd;
    curPlData->swapDesc.Windowed = true;
    curPlData->swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;                          // DXGI_SWAP_EFFECT_FLIP_DISCARD
    curPlData->swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_RESTRICTED_TO_ALL_HOLOGRAPHIC_DISPLAYS; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    curPlData->swapDesc.SampleDesc.Count = 1;
    curPlData->swapDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};
    auto res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, D3D11_CREATE_DEVICE_DEBUG,
        featureLevel, 1,
        D3D11_SDK_VERSION, &(curPlData->swapDesc),
        &(curPlData->swapChain), &(curPlData->device), nullptr,
        &(curPlData->context));

    if (FAILED(res))
    {
        // Well, that was unexpected
    }
}

void Engine::CreateTargetViewAndViewport()
{
    HRESULT res;
    ID3D11Texture2D* backTex;
    res = curPlData->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex); // __uuidof(ID3D11Texture2D)
    res = curPlData->device->CreateRenderTargetView(backTex, nullptr, &curPlData->renderTargetView);

    uint32_t row = curPlData->viewportsNum == 1 ? 1 : 2;
    uint32_t col = curPlData->viewportsNum / row + curPlData->viewportsNum % row;
    float width = static_cast<float>(curPlData->display->screenWidth / col);
    float height = static_cast<float>(curPlData->display->screenHeight / row);

    for (int32_t i = 0; i < curPlData->viewportsNum; i++)
    {
        curPlData->viewports[i] = {};
        curPlData->viewports[i].Width = width;
        curPlData->viewports[i].Height = height;
        curPlData->viewports[i].TopLeftX = (i / col) * width;  //(i * col) * width;
        curPlData->viewports[i].TopLeftY = (i % row) * height; //(i % row) * height;
        curPlData->viewports[i].MinDepth = 0;
        curPlData->viewports[i].MaxDepth = 1.0f;
    }
}

void Engine::CreateDepthStencilView()
{
    HRESULT res;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = curPlData->display->screenWidth;
    descDepth.Height = curPlData->display->screenHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D24_UNORM_S8_UINT
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    res = curPlData->device->CreateTexture2D(&descDepth, nullptr, &curPlData->depthStencil);
    if (FAILED(res)) return;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    res = curPlData->device->CreateDepthStencilView(curPlData->depthStencil, &descDSV, &curPlData->depthStencilView);
    if (FAILED(res)) return;
}

void Engine::CreateLightsBuffer()
{
    HRESULT hr;
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    constBufDesc.ByteWidth = sizeof(LightData) * lightComponents.size();
    constBufDesc.StructureByteStride = sizeof(LightData);
    hr = GetDevice()->CreateBuffer(&constBufDesc, nullptr, &lightsBuffer);
    if (FAILED(hr)) return;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = lightComponents.size();
    hr = GetDevice()->CreateShaderResourceView(lightsBuffer, &srvDesc, &lightsSRV);
    if (FAILED(hr)) return;
}
