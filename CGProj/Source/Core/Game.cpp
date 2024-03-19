#include "Game.h"
#include "Windisplay.h"

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#include <d3d.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <chrono>

#include "Object.h"
#include "../Components/GameComponent.h"
#include "../Core/Input/InputDevice.h"
#include "../Game/GameSquare.h"
#include "../Game/Camera.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


Camera* Game::CreateCamera(ViewType ViewType)
{
    //auto cam = CreateObject<Camera>();
    auto cam = new Camera();
    cam->viewType = ViewType;
    cameras.insert(cam);
    return cam;
}

Game::Game()
{
}

Game::~Game()
{
    Destroy();
}

void Game::Destroy()
{
    for (const auto gObj : gameObjects)
    {
        delete gObj;
    }

    for (const auto gComp : gameComponents)
    {
        delete gComp;
    }
    for (const auto cam : cameras)
    {
        delete cam;
    }
    for (auto plData : pipelinesData)
    {
        plData->renderTargetView->Release();
        plData->depthStencil->Release();
        plData->depthStencilView->Release();
        plData->swapChain->Release();
        plData->context->Release();
        plData->device->Release();
    }
}

Game& Game::GetGame()
{
    static Game game;
    return game;
}

int32_t Game::GetIdxCurrentPipeline()
{
    for (int32_t i = 0; i < pipelinesData.size(); i++)
    {
        if (pipelinesData[i] == curPlData) return i;
    }
    return -1;
}

void Game::AddWindow(int32_t scrWidth, int32_t scrHeight, int32_t posX, int32_t posY, const Array<Camera*>& cameras)
{
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

void Game::AddWindow(int32_t scrWidth, int32_t scrHeight, int32_t posX, int32_t posY, ViewType vType)
{
    auto cam = CreateCamera();
    cam->viewType = vType;
    Array<Camera*> cameras{};
    cameras.insert(cam);
    AddWindow(scrWidth, scrHeight, posX, posY, cameras);
}


void Game::Initialize()
{
    if (pipelinesData.isEmpty()) AddWindow();

    inputDevice = new InputDevice(&GetGame());
    if (!GetDisplay() || !inputDevice) return;

    for (auto plData : pipelinesData)
    {
        curPlData = plData;
        curPlData->display->WindowInit();
        CreateDeviceAndSwapChain();
        CreateTargetViewAndViewport();
        CreateDepthStencilView();

        for (const auto Comp : gameComponents)
        {
            Comp->Initialize();
        }
        for (const auto Obj : gameObjects)
        {
            Obj->Initialize();
        }
        for (const auto cam : cameras)
        {
            cam->Initialize();
        }
    }
}

void Game::Run()
{
    PrevTime = std::chrono::steady_clock::now();

    bool isExitRequested = false;
    while (!isExitRequested)
    {
        for (auto plData : pipelinesData)
        {
            curPlData = plData;
            Input(isExitRequested);
            Update();
            Render();
        }
    }

    std::cout << "End!\n";
}

void Game::Input(bool& isExitRequested)
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

void Game::Update()
{

    curTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
    PrevTime = curTime;

    totalTime += deltaTime;
    frameCount++;

    if (totalTime > 1.0f)
    {
        float fps = frameCount / totalTime;

        totalTime -= 1.0f;

        WCHAR text[256];
        swprintf_s(text, TEXT("FPS: %f"), fps);
        SetWindowText(GetDisplay()->hWnd, text);

        frameCount = 0;
    }

    DetectOverlapped();

    for (const auto Comp : gameComponents)
    {
        Comp->Update(deltaTime);
    }

    for (const auto Obj : gameObjects)
    {
        Obj->Update(deltaTime);
    }
    for (const auto cam : cameras)
    {
        cam->Update(deltaTime);
    }

}

void Game::DetectOverlapped()
{
    for (int32_t i = 0; i < gameObjects.size(); i++)
    {
        const auto square1 = dynamic_cast<GameSquare*>(gameObjects[i]);
        if (!square1) continue;
        for (int32_t j = i + 1; j < gameObjects.size(); j++)
        {
            const auto square2 = dynamic_cast<GameSquare*>(gameObjects[j]);
            if (!square2) continue;
            if (square1->BoxCollision.Contains(square2->BoxCollision))
            {
                square1->beginOverlapped.Broadcast(square2);
                square2->beginOverlapped.Broadcast(square1);
            }
            else
            {
                square1->endOverlapped.Broadcast(square2);
                square2->endOverlapped.Broadcast(square1);
            }
        }
    }
}

void Game::Render()
{
    float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    curPlData->context->ClearState();
    curPlData->context->ClearRenderTargetView(curPlData->renderTargetView, color);
    curPlData->context->ClearDepthStencilView(curPlData->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    curPlData->context->OMSetRenderTargets(1, &curPlData->renderTargetView, curPlData->depthStencilView);

    for (int32_t i = 0; i < curPlData->viewportsNum; i++)
    {
        printf("%d \n", i);
        curPlData->curViewport = i;
        
        curPlData->context->RSSetViewports(1, &curPlData->viewports[i]);

        for (const auto Comp : gameComponents)
        {
            Comp->Draw();
        }
        for (const auto Obj : gameObjects)
        {
            Obj->Draw();
        }
        for (const auto cam : cameras)
        {
            cam->Draw();
        }
    }
    curPlData->swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);

}

void Game::CreateDeviceAndSwapChain()
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
    curPlData->swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;     // DXGI_SWAP_EFFECT_FLIP_DISCARD
    curPlData->swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
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

void Game::CreateTargetViewAndViewport()
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
        curPlData->viewports[i].TopLeftX = (i / col) * width; //(i * col) * width;
        curPlData->viewports[i].TopLeftY = (i % row) * height; //(i % row) * height;
        curPlData->viewports[i].MinDepth = 0;
        curPlData->viewports[i].MaxDepth = 1.0f;
    }
}

void Game::CreateDepthStencilView()
{
    HRESULT res;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = static_cast<float>(curPlData->display->screenWidth);
    descDepth.Height = static_cast<float>(curPlData->display->screenHeight);
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

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    res = curPlData->device->CreateDepthStencilView(curPlData->depthStencil, &descDSV, &curPlData->depthStencilView);
}
