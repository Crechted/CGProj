#include "TriangleComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>
#include <string>
#include <strstream>

#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Render/Shader.h"
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
        if (defShader) defShader->Destroy();
        if (cascadeShader) cascadeShader->Destroy();
        if (shadowMappingShader) shadowMappingShader->Destroy();
    }
}

void TriangleComponent::Draw()
{
    if (!engInst) return;
    if (engInst->GetRenderState() == RenderState::DrawDebug || engInst->GetRenderState() == RenderState::Deferred_Lighting) return;

    const uint32_t stride = sizeof(Vertex);
    const uint32_t offset = 0;

    UpdateData();
    engInst->GetContext()->RSSetState(engInst->GetCurEyeData().isCam ? rastState : rastStateShadow);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    curShader->Draw();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
    engInst->GetContext()->RSSetState(nullptr);
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

    /*D3D11_MAPPED_SUBRESOURCE res = {};
    engInst->GetContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<Vertex*>(res.pData);
    for (int32_t i = 0; i < vertices.size(); i++)
    {
        dataPtr[i].position = vertices[i].pos;        
        dataPtr[i].normal = vertices[i].norm;
        dataPtr[i].texture = vertices[i].tex;
    }
    engInst->GetContext()->Unmap(vertexBuffer, 0);*/
}

void TriangleComponent::Update(float timeTick)
{
    UpdateData();
    totalTime = engInst->GetTotalTime();
}

void TriangleComponent::AddVertex(const Vertex& vertex)
{
    vertices.insert(vertex);
}

void TriangleComponent::AddVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT2& tex)
{
    const Vertex vert{pos, Vector3::Zero, Vector3::Zero, norm, tex};
    vertices.insert(vert);
}

void TriangleComponent::SetVertices(const Array<Vertex>& verts)
{
    vertices.clear();
    for (const auto& vert : verts)
    {
        vertices.insert(vert);
    }
}

/*void TriangleComponent::SetVertices(const Array<D3DVertex>& pts)
{
    vertices.clear();
    vertices = pts;
}*/

void TriangleComponent::AddIndex(int32_t idx)
{
    indexes.insert(idx);
}

void TriangleComponent::SetIndexes(int32_t* idxs, int32_t count)
{
    indexes.clear();
    for (int32_t i = 0; i < count; i++)
    {
        indexes.insert(idxs[i]);
    }

}

void TriangleComponent::SetShader(Shader* difShader)
{
    curShader = difShader ? difShader : curShader;
}

void TriangleComponent::SetDefaultShader()
{
    curShader = defShader;
}

void TriangleComponent::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(Vertex) * vertices.size();

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

    rastDesc.CullMode = D3D11_CULL_FRONT; // D3D11_CULL_NONE
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.DepthBias = 100;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.SlopeScaledDepthBias = 1.0f;
    res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastStateShadow);
    engInst->GetContext()->RSSetState(rastState);
}

void TriangleComponent::OnChangeRenderState(RenderState state)
{
    switch (state)
    {
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
        case RenderState::Deferred_GBuffer:
        {
            if (!deferredGeometryPassShader) CreateDeferredGeometryShader();
            SetShader(deferredGeometryPassShader);
            break;
        }
        default:
        {
            SetDefaultShader();
            break;
        }
    }
}

void TriangleComponent::CreateDefaultShader()
{
    if (defShader) return;
    defShader = new Shader();
    defShader->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    defShader->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    defShader->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    defShader->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    defShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    std::strstream s;
    std::string cascade, numLights;
    s << CASCADE_COUNT << "\x00";
    s >> cascade;
    s.clear();
    s << engInst->GetLightComponents().size() << "\x00";
    s >> numLights;
    D3D_SHADER_MACRO* macro = engInst->useCascadeShadow
                                  ? new D3D_SHADER_MACRO[]{"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", cascade.c_str(), "NUM_LIGHTS",
                                                           numLights.c_str(), nullptr, nullptr}
                                  : new D3D_SHADER_MACRO[]{"NUM_LIGHTS", numLights.c_str(), nullptr, nullptr};
    defShader->CreateShader(pFileName, SVertex, macro);
    defShader->CreateShader(pFileName, SPixel, macro);
}

void TriangleComponent::CreateCascadeShader()
{
    cascadeShader = new Shader();
    cascadeShader->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    cascadeShader->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    cascadeShader->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    cascadeShader->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    cascadeShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

    std::strstream s;
    std::string cascade, numLights;
    s << CASCADE_COUNT << "\x00";
    s >> cascade;
    s.clear();
    s << engInst->GetLightComponents().size() << "\x00";
    s >> numLights;
    D3D_SHADER_MACRO macro[] = {"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", cascade.c_str(), "NUM_LIGHTS", numLights.c_str(), nullptr,
                                nullptr};
    cascadeShader->CreateShader(L"./Resource/Shaders/CascadeShadowShader.hlsl", SVertex, macro);
    cascadeShader->CreateShader(L"./Resource/Shaders/CascadeShadowShader.hlsl", SGeometry, macro);
}

void TriangleComponent::CreateShadowMappingShader()
{
    shadowMappingShader = new Shader();
    shadowMappingShader->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    shadowMappingShader->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    shadowMappingShader->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    shadowMappingShader->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    shadowMappingShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    shadowMappingShader->CreateShader(L"./Resource/Shaders/ShadowMappingShader.hlsl", SVertex);
}


void TriangleComponent::CreateDeferredGeometryShader()
{
    deferredGeometryPassShader = new Shader();
    deferredGeometryPassShader->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    deferredGeometryPassShader->AddInputElementDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    deferredGeometryPassShader->AddInputElementDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    deferredGeometryPassShader->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
    deferredGeometryPassShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    deferredGeometryPassShader->CreateShader(L"./Resource/Shaders/GBufferShader.hlsl", SVertex);
    deferredGeometryPassShader->CreateShader(L"./Resource/Shaders/GBufferShader.hlsl", SPixel);
}
