#include "TriangleComponent.h"

#include <windows.h>
#include <WinUser.h>
#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>

#include "../Core/Game.h"
#include "../Core/Windisplay.h"


TriangleComponent::TriangleComponent()
{
}

TriangleComponent::~TriangleComponent()
{
    DestroyResource();
}

void TriangleComponent::Initialize()
{
    const auto& swapDesc = game->swapDesc;
    auto& swapChain = game->swapChain;
    auto& device = game->device;
    auto& context = game->context;
    const auto& display = game->display;

    ID3D11Texture2D* backTex;
    auto res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex); // __uuidof(ID3D11Texture2D)
    res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

    vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    res = D3DCompileFromFile(L"./Resource/Shaders/MyVeryFirstShader.hlsl", nullptr /*macros*/, nullptr /*include*/, "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC, &errorVertexCode);

    if (FAILED(res))
    {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode)
        {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(display->hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
        }

        return;
    }

    D3D_SHADER_MACRO Shader_Macros[] = {"TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};

    ID3DBlob* errorPixelCode;
    res = D3DCompileFromFile(L"./Resource/Shaders/MyVeryFirstShader.hlsl", Shader_Macros /*macros*/, nullptr /*include*/, "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

    device->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &vertexShader);

    device->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &pixelShader);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        D3D11_INPUT_ELEMENT_DESC{
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    device->CreateInputLayout(inputElements, 2, vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), &layout);

    /*DirectX::XMFLOAT4 points[8] = {
        DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    };*/

    //D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * countPoints;

    //D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = points;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    
    device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

    int indeces[] = {0, 1, 2, 1, 0, 3};
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    
    device->CreateBuffer(&indexBufDesc, &indexData, &ib);

    strides = new UINT[1]{32};
    offsets = new UINT[1]{0};

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    rastState;
    res = device->CreateRasterizerState(&rastDesc, &rastState);

    context->RSSetState(rastState);

    //       FROM CYCLE
    //Draw();
}

void TriangleComponent::DestroyResource()
{
    vertexShader->Release();
    pixelShader->Release();
}

void TriangleComponent::Draw()
{
    if (!game) return;
    
    float color[] = {totalTime, 0.1f, 0.1f, 1.0f};
    game->context->ClearState();

    game->context->RSSetState(rastState);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(game->display->screenWidth);
    viewport.Height = static_cast<float>(game->display->screenHeight);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    game->context->RSSetViewports(1, &viewport);

    game->context->IASetInputLayout(layout);
    game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
    game->context->VSSetShader(vertexShader, nullptr, 0);
    game->context->PSSetShader(pixelShader, nullptr, 0);

    game->context->OMSetRenderTargets(1, &rtv, nullptr);
    game->context->ClearRenderTargetView(rtv, color);
    game->context->DrawIndexed(6, 0, 0);

    game->context->OMSetRenderTargets(0, nullptr, nullptr);
    game->swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void TriangleComponent::Reload()
{
}

void TriangleComponent::Update(float timeTick)
{
    totalTime = game->GetTotalTime();
}
