#include "Game.h"
#include "Windisplay.h"

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#include <d3d.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <chrono>

#include "../Components/GameComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

Game::~Game()
{
    for (const auto gComp : gameComponents)
    {
        delete gComp;
    }
}

Game &Game::GetGame()
{
    static Game game;
    return game;
}

void Game::Initialize()
{
    display = new WinDisplay();
    display->WindowInit();
    if (!display)
        return;

    //swapDesc = {};
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

void Game::Input(bool &isExitRequested)
{
    // Handle the windows messages.
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // If windows signals to end the application then exit out.
    if (msg.message == WM_QUIT)
    {
        isExitRequested = true;
    }
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

void Game::Update()
{
    curTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<
            std::chrono::microseconds>(curTime - PrevTime).count() /
        1000000.0f;
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

    for (const auto gComp : gameComponents)
    {
        gComp->Update(deltaTime);
    }
}

void Game::Render()
{
    float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    context->ClearState();
    context->RSSetViewports(1, &viewport);
    context->ClearRenderTargetView(renderTargetView, color);
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);
    
    for (const auto gComp : gameComponents)
    {
        gComp->Draw();
    }
    
    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}
