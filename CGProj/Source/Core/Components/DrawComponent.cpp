#include "DrawComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>
#include <string>
#include <strstream>

#include "TriangleComponent.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"
#include "Core/Render/Shader.h"

void DrawComponent::Initialize()
{
    CreateDefaultShader();
    SetDefaultShader();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateAndSetRasterizerState();
}

void DrawComponent::DestroyResource()
{
        if (rastState)rastState->Release();
        if (vertexBuffer)vertexBuffer->Release();
        if (indexBuffer)indexBuffer->Release();

        if (defShader) defShader->Destroy();
        if (cascadeShader) cascadeShader->Destroy();
        if (shadowMappingShader) shadowMappingShader->Destroy();
    
}

void DrawComponent::Draw()
{
    if (!engInst) return;
    if (engInst->GetRenderState() != RenderState::DrawDebug) return;

    const uint32_t strides = sizeof(VertexNoTex);
    const uint32_t offsets = 0;
    UpdateData();
    engInst->GetContext()->RSSetState(rastState);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
    curShader->BindShaders();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
    engInst->GetContext()->RSSetState(nullptr);
}

void DrawComponent::Reload()
{
}

void DrawComponent::UpdateData()
{

    D3D11_MAPPED_SUBRESOURCE res = {};
    engInst->GetContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<VertexNoTex*>(res.pData);
    for (int32_t i = 0; i < vertices.size(); i++)
    {
        dataPtr[i].position = vertices[i].position;
        dataPtr[i].color = vertices[i].color;
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
    vertices.insert(vertex);
}

void DrawComponent::AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& col)
{
    const VertexNoTex vert{pos, col};
    vertices.insert(vert);
}

void DrawComponent::SetVertices(const Array<VertexNoTex>& verts)
{
    vertices.clear();
    for (const auto& vert : verts)
    {
        vertices.insert(vert);
    }
}

void DrawComponent::SetVertices(const Array<DirectX::XMFLOAT4>& pts)
{
    vertices.clear();
    for (int32_t i = 0; i < pts.size() - 1; i += 2)
    {
        const VertexNoTex tempVert{Vector4(pts[i].x, pts[i].y, pts[i].z, pts[i].w),
                                   Vector4(pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w)};
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
        const VertexNoTex tempVert{Vector4(pts[i].x, pts[i].y, pts[i].z, pts[i].w),
                                   Vector4(pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w)};
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
    std::strstream s;
    std::string res;
    s << CASCADE_COUNT << "\x00";
    s >> res;
    D3D_SHADER_MACRO* macro = engInst->useCascadeShadow
                                  ? new D3D_SHADER_MACRO[3]{"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", res.c_str(), nullptr, nullptr}
                                  : nullptr;
    defShader->CreateShader(pFileName, SVertex, macro);
    defShader->CreateShader(pFileName, SPixel, macro);
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
    vertexBufDesc.ByteWidth = sizeof(VertexNoTex) * vertices.size();

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

    D3D11_SUBRESOURCE_DATA indexData;
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
