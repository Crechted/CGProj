﻿#include "PostProcess.h"

#include "RenderTarget.h"
#include "Core/Engine.h"
#include "Core/Render/Shader.h"

PostProcess::PostProcess(int32_t screenW, int32_t screenH, LPCWSTR shaderPath)
    : screenWidth(screenW), screenHeight(screenH), shaderPath(shaderPath)
{
    signed int centreW = screenWidth / 2 * -1;
    signed int centreH = screenHeight / 2;
    float left = (float)centreW;
    float right = left + screenWidth;
    float top = (float)centreH;
    float bottom = top - screenHeight;

    vertices.insert({Vector4(left, top, 0.0f, 1.0f), Vector4::Zero, Vector2(0.0f, 0.0f)});
    vertices.insert({Vector4(right, top, 0.0f, 1.0f), Vector4::Zero, Vector2(1.0f, 1.0f)});
    vertices.insert({Vector4(left, bottom, 0.0f, 1.0f), Vector4::Zero, Vector2(0.0f, 1.0f)});
    vertices.insert({Vector4(right, bottom, 0.0f, 1.0f), Vector4::Zero, Vector2(1.0f, 0.0f)});

    indexes.insert(0);
    indexes.insert(1);
    indexes.insert(3);
    indexes.insert(0);
    indexes.insert(3);
    indexes.insert(2);

    renderTarget = new RenderTarget(TargetViewType::Texture, screenWidth, screenHeight);    
}

void PostProcess::Update(float timeTick)
{
    Object::Update(timeTick);
    /*for (int32_t i = 0; i < vertices.size(); i++)
    {
        vertices[i].WorldPos.w = engInst->GetTotalTime();
    }*/
    ConstBufferData data{Vector2(engInst->GetTotalTime()) };
    engInst->GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &data, 0, 0);
}

void PostProcess::Draw()
{
    const uint32_t stride = sizeof(PostProcessVertex);
    const uint32_t offset = 0;

    D3D11_MAPPED_SUBRESOURCE res = {};
    /*engInst->GetContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<PostProcessVertex*>(res.pData);
    for (int32_t i = 0; i < vertices.size(); i++)
    {
        dataPtr[i].pos = vertices[i].pos;        
        dataPtr[i].WorldPos = vertices[i].WorldPos;
        dataPtr[i].tex = vertices[i].tex;
    }
    engInst->GetContext()->Unmap(vertexBuffer, 0);*/
    
    renderTarget->SetTarget();
    renderTarget->ClearTarget();
    engInst->GetContext()->PSSetConstantBuffers(0, 1, &constBuffer);
    engInst->GetContext()->RSSetState(nullptr);
    engInst->GetContext()->PSSetShaderResources(0, 1, &texSRV);
    //engInst->GetContext()->PSSetSamplers(0, 1, &);
    //engInst->GetContext()->RSSetState(rastState);
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    shader->Draw();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void PostProcess::CreateShader()
{
    shader = new Shader();
    shader->AddInputElementDesc("POSITION");
    shader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    shader->CreateShader(shaderPath, ShaderType::Vertex);
    shader->CreateShader(shaderPath, ShaderType::Pixel);
}

void PostProcess::Initialize()
{
    D3D11_BUFFER_DESC constBufDesc{};
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(ConstBufferData);
    const auto hr = engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &constBuffer);
    
    CreateShader();
    CreateVertexBuffer();
    CreateIndexBuffer();
    
    renderTarget->Initialize();
    Object::Initialize();
}

void PostProcess::Destroy()
{
    Object::Destroy();
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (constBuffer) constBuffer->Release();
    delete renderTarget;
}

void PostProcess::SetSRV(ID3D11ShaderResourceView* srv)
{
    if (!srv) return;
    texSRV = srv;
}

void PostProcess::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(PostProcessVertex) * vertices.size();

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
}

void PostProcess::CreateIndexBuffer()
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