#include "DrawComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>
#include <string>

#include "TriangleComponent.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"
#include "Game/Pong/Components/Movement2DComponent.h"




void DrawComponent::Initialize()
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
    
    engInst->OnChangeRenderStateDelegate.AddRaw(this, &DrawComponent::OnChangeRenderState);
}

void DrawComponent::DestroyResource()
{
    if (curDrawData)
    {
        if (curDrawData->rastState) curDrawData->rastState->Release();
        if (curDrawData->layout) curDrawData->layout->Release();
        if (curDrawData->vertexShader) curDrawData->vertexShader->Release();
        if (curDrawData->pixelShader) curDrawData->pixelShader->Release();
        if (curDrawData->vertexBuffer) curDrawData->vertexBuffer->Release();
        if (curDrawData->indexBuffer) curDrawData->indexBuffer->Release();
        curShader->Destroy();
        defShader->Destroy();
        cascadeShader->Destroy();
        shadowMappingShader->Destroy();
    }
}

void DrawComponent::Draw()
{
    if (!engInst) return;
    
    const uint32_t strides = sizeof(VertexNoTex);
    const uint32_t offsets = 0;
    UpdateData();
    engInst->GetContext()->RSSetState(engInst->GetCurEyeData().isCam ? rastState : nullptr);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
    curShader->Draw();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void DrawComponent::Reload()
{
}

void DrawComponent::UpdateData()
{
    //curDrawData = drawsData[engInst->GetIdxCurrentPipeline()];
    rastState = curDrawData->rastState;
    vertexBuffer = curDrawData->vertexBuffer;
    indexBuffer = curDrawData->indexBuffer;

    D3D11_MAPPED_SUBRESOURCE res = {};
    
    engInst->GetContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<VertexNoTex*>(res.pData);
    for (int32_t i = 0; i < vertices.size(); i++)
    {
        dataPtr[i].position = vertices[i].pos;        
        dataPtr[i].color = vertices[i].col;
    }
    engInst->GetContext()->Unmap(vertexBuffer, 0);
}

void DrawComponent::Update(float timeTick)
{
    UpdateData();
    totalTime = engInst->GetTotalTime(); 
}

void DrawComponent::AddVertex(const VertexNoTex& vertex)
{
    const D3DMinVertex vert{{vertex.position.x, vertex.position.y, vertex.position.z, vertex.position.w},
                            {vertex.color.x, vertex.color.y, vertex.color.z, vertex.color.w}};
    vertices.insert(vert);
}

void DrawComponent::AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& col)
{
    const D3DMinVertex vert{pos, col};
    vertices.insert(vert);
}

void DrawComponent::SetVertices(const Array<VertexNoTex>& verts)
{
    vertices.clear();
    for (const auto& vert : verts)
    {
        const D3DMinVertex tempVert{{vert.position.x, vert.position.y, vert.position.z, vert.position.w},
                                    {vert.color.x, vert.color.y, vert.color.z, vert.color.w}};
        vertices.insert(tempVert);
    }
}

void DrawComponent::SetVertices(const Array<DirectX::XMFLOAT4>& pts)
{
    vertices.clear();
    for (int32_t i = 0; i < pts.size() - 1; i += 2)
    {
        const D3DMinVertex tempVert{{pts[i].x, pts[i].y, pts[i].z, pts[i].w},
                                    {pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w},};
        vertices.insert(tempVert);
    }
}

void DrawComponent::AddIndex(int32_t idx)
{
    indexes.insert(idx);
}

void DrawComponent::SetVertices(DirectX::XMFLOAT4* pts, int32_t count)
{
    vertices.clear();
    for (int32_t i = 0; i < count - 1; i += 2)
    {
        const D3DMinVertex tempVert{{pts[i].x, pts[i].y, pts[i].z, pts[i].w},
                                    {pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w},};
        vertices.insert(tempVert);
    }
}

void DrawComponent::SetShader(Shader* difShader)
{
    if (difShader) curShader = difShader;
}

void DrawComponent::SetDefaultShader()
{
    curShader = defShader;
}

void DrawComponent::CreateDefaultShader()
{
    if (defShader) return;
    defShader = new Shader();
    defShader->AddInputElementDesc("POSITION");
    defShader->AddInputElementDesc("COLOR");
    D3D_SHADER_MACRO* macro = engInst->useCascadeShadow ? new D3D_SHADER_MACRO[3]{"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", std::to_string(engInst->CASCADE_COUNT).c_str(), nullptr, nullptr} : nullptr;
    defShader->CreateShader(pFileName, ShaderType::Vertex, macro);
    defShader->CreateShader(pFileName, ShaderType::Pixel, macro);
}

void DrawComponent::OnChangeRenderState(RenderState state)
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
        default: break;
    }
}

void DrawComponent::CreateCascadeShader()
{
    cascadeShader = new Shader();
    cascadeShader->AddInputElementDesc("POSITION");
    cascadeShader->AddInputElementDesc("COLOR");
    D3D_SHADER_MACRO macro[] = {"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", std::to_string(engInst->CASCADE_COUNT).c_str(), nullptr, nullptr};
    cascadeShader->CreateShader(L"./Resource/Shaders/CascadeShadowShader.hlsl", ShaderType::Vertex, macro);
    //cascadeShader->CreateShader(L"./Resource/Shaders/CascadeShadowShader.hlsl", ShaderType::Geometry, macro);
}

void DrawComponent::CreateShadowMappingShader()
{
    shadowMappingShader = new Shader();
    shadowMappingShader->AddInputElementDesc("POSITION");
    shadowMappingShader->AddInputElementDesc("COLOR");
    shadowMappingShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", ShaderType::Vertex);
}

void DrawComponent::SetIndexes(int32_t* idxs, int32_t count)
{
    indexes.clear();
    for (int32_t i = 0; i < count; i++)
    {
        indexes.insert(idxs[i]);
    }
}

void DrawComponent::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(D3DMinVertex) * vertices.size();

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
}

void DrawComponent::CreateIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufDesc;
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

void DrawComponent::CreateAndSetRasterizerState()
{
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = cullMode; // D3D11_CULL_NONE
    rastDesc.FillMode = fillMode; // D3D11_FILL_SOLID
    rastDesc.AntialiasedLineEnable = isAntialiasedLine;

    auto res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastState);
}
