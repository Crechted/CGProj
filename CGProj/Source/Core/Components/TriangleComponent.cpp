#include "TriangleComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>

#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"


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

    curDrawData = new TriangleDrawData();
    curDrawData->rastState = rastState;
    curDrawData->layout = layout;
    curDrawData->vertexShader = vertexShader;
    curDrawData->pixelShader = pixelShader;
    curDrawData->vertexBuffer = vertexBuffer;
    curDrawData->indexBuffer = indexBuffer;
    drawsData.insert(engInst->GetIdxCurrentPipeline(), curDrawData);
}

void TriangleComponent::DestroyResource()
{
    for (const auto element : drawsData)
    {
        if (element->rastState) element->rastState->Release();
        if (element->layout) element->layout->Release();
        if (element->vertexShader) element->vertexShader->Release();
        if (element->pixelShader) element->pixelShader->Release();
        if (element->vertexBuffer) element->vertexBuffer->Release();
        if (element->indexBuffer) element->indexBuffer->Release();
        if (vertexBC) vertexBC->Release();
        if (pixelBC) pixelBC->Release();
    }
}

void TriangleComponent::PreDraw()
{
    if (!engInst) return;
    UpdateData();
    engInst->GetContext()->RSSetState(rastState);
    engInst->GetContext()->IASetInputLayout(layout);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
    engInst->GetContext()->VSSetShader(vertexShader, nullptr, 0);
    engInst->GetContext()->PSSetShader(pixelShader, nullptr, 0);
}

void TriangleComponent::Draw()
{
    if (!engInst) return;
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void TriangleComponent::Reload()
{
}

void TriangleComponent::UpdateData()
{
    curDrawData = drawsData[engInst->GetIdxCurrentPipeline()];
    rastState = curDrawData->rastState;
    layout = curDrawData->layout;
    vertexShader = curDrawData->vertexShader;
    pixelShader = curDrawData->pixelShader;
    vertexBuffer = curDrawData->vertexBuffer;
    indexBuffer = curDrawData->indexBuffer;
}

void TriangleComponent::Update(float timeTick)
{
    UpdateData();
    totalTime = engInst->GetTotalTime();
}

void TriangleComponent::AddVertex(const Vertex& vertex)
{
    const D3DVertex tempVert{{vertex.position.x, vertex.position.y, vertex.position.z, vertex.position.w},
                             {vertex.normal.x, vertex.normal.y, vertex.normal.z, vertex.normal.w},
                             {vertex.texture.x, vertex.texture.y}};
    vertices.insert(tempVert);
}

void TriangleComponent::AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& norm, const DirectX::XMFLOAT2& tex)
{
    const D3DVertex vert{pos, norm, tex};
    vertices.insert(vert);
}

void TriangleComponent::SetVertices(const Array<Vertex>& verts)
{
    for (const auto& vert : verts)
    {
        const D3DVertex tempVert{{vert.position.x, vert.position.y, vert.position.z, vert.position.w},
                                 {vert.normal.x, vert.normal.y, vert.normal.z, vert.normal.w},
                                 {vert.texture.x, vert.texture.y}};
        vertices.insert(tempVert);
    }
}

void TriangleComponent::SetVertices(const Array<D3DVertex>& pts)
{
    vertices = pts;
}

void TriangleComponent::AddIndex(int32_t idx)
{
    indexes.insert(idx);
}

void TriangleComponent::SetIndexes(int32_t* idxs, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        indexes.insert(idxs[i]);
    }

}

bool TriangleComponent::CompileVertexBC()
{
    vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    const auto res = D3DCompileFromFile(pFileName, nullptr, nullptr,
        VSname, "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC,
        &errorVertexCode);

    return CheckResCompile(errorVertexCode, res, pFileName);
}

bool TriangleComponent::CompilePixelBC()
{
    D3D_SHADER_MACRO Shader_Macros[] = {"TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};
    ID3DBlob* errorPixelCode;

    auto res = D3DCompileFromFile(pFileName, Shader_Macros,
        nullptr, PSname,
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
            MessageBox(engInst->GetDisplay()->hWnd, pFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    return true;
}

void TriangleComponent::CreateShaders()
{
    engInst->GetDevice()->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &vertexShader);
    engInst->GetDevice()->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &pixelShader);
}

void TriangleComponent::CreateLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        D3D11_INPUT_ELEMENT_DESC{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
                                 0},
        D3D11_INPUT_ELEMENT_DESC{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    engInst->GetDevice()->CreateInputLayout(inputElements, numElements, vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(),
        &layout);
}

void TriangleComponent::CreateVertexBuffer()
{
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(D3DVertex) * vertices.size();

    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
}

void TriangleComponent::CreateIndexBuffer()
{
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * indexes.size();

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = &indexes[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);
}

void TriangleComponent::CreateAndSetRasterizerState()
{
    strides = new UINT[1]{40};
    offsets = new UINT[1]{0};

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = cullMode; // D3D11_CULL_NONE
    rastDesc.FillMode = fillMode; // D3D11_FILL_SOLID
    rastDesc.AntialiasedLineEnable = isAntialiasedLine;

    auto res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastState);
    //engInst->GetContext()->RSSetState(rastState);
}
