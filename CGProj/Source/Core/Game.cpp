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
#include "../Core/Input/InputDevice.h"
#include "../Game/GameSquare.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

Game::~Game()
{
    for (const auto gComp : gameObjects)
    {
        delete gComp;
    }
    Destroy();
}

void Game::Destroy()
{
    renderTargetView->Release();
    swapChain->Release();
    context->Release();
    device->Release();
}

Game& Game::GetGame()
{
    static Game game;
    return game;
}

void Game::Initialize()
{
    display = new WinDisplay();
    inputDevice = new InputDevice(&GetGame());
    if (!display || !inputDevice) return;
    display->WindowInit();

    CreateDeviceAndSwapChain();
    CreateTargetViewAndViewport();
}

void Game::Run()
{
    PrevTime = std::chrono::steady_clock::now();

    bool isExitRequested = false;
    while (!isExitRequested)
    {
        Input(isExitRequested);
        Update();
        Render();
    }

    std::cout << "Hello World!\n";
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
        SetWindowText(display->hWnd, text);

        frameCount = 0;
    }

    DetectOverlapped();

    for (const auto Obj : gameObjects)
    {
        Obj->Update(deltaTime);
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
    context->ClearState();
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);
    context->ClearRenderTargetView(renderTargetView, color);
    context->RSSetViewports(1, &viewport);

    for (const auto Obj : gameObjects)
    {
        Obj->Draw();
    }

    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::CreateDeviceAndSwapChain()
{
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = display->screenWidth;
    swapDesc.BufferDesc.Height = display->screenHeight;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = display->hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};
    auto res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, D3D11_CREATE_DEVICE_DEBUG,
        featureLevel, 1,
        D3D11_SDK_VERSION, &swapDesc,
        &swapChain, &device, nullptr,
        &context);

    if (FAILED(res))
    {
        // Well, that was unexpected
    }
}

void Game::CreateTargetViewAndViewport()
{
    HRESULT res;
    ID3D11Texture2D* backTex;
    res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex); // __uuidof(ID3D11Texture2D)
    res = device->CreateRenderTargetView(backTex, nullptr, &renderTargetView);

    viewport = {};
    viewport.Width = static_cast<float>(display->screenWidth);
    viewport.Height = static_cast<float>(display->screenHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;
}
