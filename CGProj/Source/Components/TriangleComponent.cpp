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
    if (!CompileVertexBC())
        return;

    if (!CompilePixelBC())
        return;

    CreateShaders();
    CreateLayout();

    CreateVertexBuffer();

    CreateIndexBuffer();

    CreateAndSetRasterizerState();    
}

void TriangleComponent::DestroyResource()
{
    vertexShader->Release();
    pixelShader->Release();
}

void TriangleComponent::Draw()
{
    if (!game)
        return;
    
    game->context->RSSetState(rastState);

    game->context->IASetInputLayout(layout);
    game->context->IASetPrimitiveTopology(topology);
    game->context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);
    game->context->VSSetShader(vertexShader, nullptr, 0);
    game->context->PSSetShader(pixelShader, nullptr, 0);
    

    game->context->DrawIndexed(idxCount, 0, 0);
}

void TriangleComponent::Reload()
{
}

void TriangleComponent::Update(float timeTick)
{
    totalTime = game->GetTotalTime();
}

bool TriangleComponent::CompileVertexBC()
{
    vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    LPCWSTR pFileName = L"./Resource/Shaders/MyVeryFirstShader.hlsl";
    const auto res = D3DCompileFromFile(pFileName, nullptr /*macros*/, nullptr /*include*/,
        "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC,
        &errorVertexCode);

    return CheckResCompile(errorVertexCode, res, pFileName);
}

bool TriangleComponent::CompilePixelBC()
{
    D3D_SHADER_MACRO Shader_Macros[] = {"TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};
    ID3DBlob* errorPixelCode;

    LPCWSTR pFileName = L"./Resource/Shaders/MyVeryFirstShader.hlsl";
    auto res = D3DCompileFromFile(pFileName, Shader_Macros /*macros*/,
        nullptr /*include*/, "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
    return CheckResCompile(errorPixelCode, res, pFileName);
}

bool TriangleComponent::CheckResCompile(ID3DBlob* errorVertexCode, const HRESULT& res, LPCWSTR pFileName)
{
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
            MessageBox(game->display->hWnd, pFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    return true;
}

void TriangleComponent::CreateShaders()
{
    game->device->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &vertexShader);
    game->device->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &pixelShader);
}

void TriangleComponent::CreateLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
                                 D3D11_INPUT_PER_VERTEX_DATA, 0}};

    game->device->CreateInputLayout(inputElements, numElements, vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(),
        &layout);
}

void TriangleComponent::CreateVertexBuffer()
{
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * countPoints;

    vertexData.pSysMem = points;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    game->device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);
}

void TriangleComponent::CreateIndexBuffer()
{
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * idxCount;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indexes;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    game->device->CreateBuffer(&indexBufDesc, &indexData, &ib);
}

void TriangleComponent::CreateAndSetRasterizerState()
{
    strides = new UINT[1]{32};
    offsets = new UINT[1]{0};

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    auto res = game->device->CreateRasterizerState(&rastDesc, &rastState);

    game->context->RSSetState(rastState);
}
