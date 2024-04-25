#include "TriangleComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>

#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Shader.h"
#include "Core/Windisplay.h"


void TriangleComponent::Initialize()
{
    CreateDefaultShader();
    SetDefaultShader();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateAndSetRasterizerState();

    curDrawData = new TriangleDrawData();
    curDrawData->rastState = rastState;
    curDrawData->vertexBuffer = vertexBuffer;
    curDrawData->indexBuffer = indexBuffer;
    //drawsData.insert(engInst->GetIdxCurrentPipeline(), curDrawData);
    engInst->OnChangeRenderStateDelegate.AddRaw(this, &TriangleComponent::OnChangeRenderState);
}

void TriangleComponent::DestroyResource()
{
    if (curDrawData)
    {
        if (curDrawData->rastState) curDrawData->rastState->Release();
        //if (curDrawData->layout) curDrawData->layout->Release();
        if (curDrawData->vertexBuffer) curDrawData->vertexBuffer->Release();
        if (curDrawData->indexBuffer) curDrawData->indexBuffer->Release();
    }
}

void TriangleComponent::Draw()
{
    if (!engInst) return;

    const uint32_t stride = sizeof(Vertex);
    const uint32_t offset = 0;

    UpdateData();
    engInst->GetContext()->RSSetState(engInst->GetCurEyeData().isCam ? rastState : rastStateShadow);
    //engInst->GetContext()->RSSetState(rastState);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    curShader->Draw();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void TriangleComponent::Reload()
{
}

void TriangleComponent::UpdateData()
{
    //curDrawData = drawsData[engInst->GetIdxCurrentPipeline()];
    rastState = curDrawData->rastState;
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

void TriangleComponent::SetShader(Shader* difShader)
{
    curShader = difShader;
}

void TriangleComponent::SetDefaultShader()
{
    curShader = defShader;
}

void TriangleComponent::CreateDefaultShader()
{
    if (defShader) return;
    defShader = new Shader();
    defShader->AddInputElementDesc("POSITION");
    defShader->AddInputElementDesc("NORMAL");
    defShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    defShader->CreateShader(pFileName, ShaderType::Vertex);
    defShader->CreateShader(pFileName, ShaderType::Pixel);
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

void TriangleComponent::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(D3DVertex) * vertices.size();

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
}

void TriangleComponent::CreateIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufDesc;
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * indexes.size();

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &indexes[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);
}

void TriangleComponent::CreateAndSetRasterizerState()
{
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = cullMode; // cullMode
    rastDesc.FillMode = fillMode; // fillMode
    //rastDesc.AntialiasedLineEnable = isAntialiasedLine;
    auto res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastState);

    rastDesc.CullMode = D3D11_CULL_NONE; // D3D11_CULL_NONE
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.DepthBias = 100000;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.SlopeScaledDepthBias = 1.0f;
    res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastStateShadow);
    engInst->GetContext()->RSSetState(rastState);
}

void TriangleComponent::OnChangeRenderState(RenderState state)
{
    switch (state)
    {
        case RenderState::Normal:
        {
            SetDefaultShader();
            break;
        }
        case RenderState::ShadowMap:
        {
            if (!shadowMappingShader) CreateShadowMappingShader();
            SetShader(shadowMappingShader);
            break;
        }
        case RenderState::CascadeShadow:
        {
            if (!cascadeShader) CreateCascadeShader();
            SetShader(cascadeShader);
            break;
        }
    }
}

void TriangleComponent::CreateCascadeShader()
{
    /*cascadeShader = new Shader();
    cascadeShader->AddInputElementDesc("POSITION");
    cascadeShader->AddInputElementDesc("NORMAL");
    cascadeShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    cascadeShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", ShaderType::Vertex);
    cascadeShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", ShaderType::Pixel);*/
}

void TriangleComponent::CreateShadowMappingShader()
{
    shadowMappingShader = new Shader();
    shadowMappingShader->AddInputElementDesc("POSITION");
    shadowMappingShader->AddInputElementDesc("NORMAL");
    shadowMappingShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    shadowMappingShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", ShaderType::Vertex);
    shadowMappingShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", ShaderType::Pixel);
}

