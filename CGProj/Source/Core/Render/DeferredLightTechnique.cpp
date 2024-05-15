#include "DeferredLightTechnique.h"

#include <strstream>

#include "RenderTarget.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"
#include "Core/Components/SceneComponent.h"

DeferredLightTechnique::DeferredLightTechnique()
{
    engInst = &Engine::GetInstance();
}

void DeferredLightTechnique::Render()
{
    GBufferPass();
    LightingPass();
    engInst->GetTexRenderTarget()->CopyDepthStencilView(tarDepthStencil->GetDepthStencilView(), tarDepthStencil->GetDepthStencilSRV(), tarDepthStencil->GetDepthStencilTexture());
}

void DeferredLightTechnique::Initialize()
{
    GBuffer.clear();

    tarDepthStencil = new RenderTarget(TargetViewType::Texture, engInst->GetDisplay()->screenWidth, engInst->GetDisplay()->screenHeight);
    tarDepthStencil->CreateDepthStencilView();

    tarLightAccumulation = new RenderTarget(TargetViewType::Texture, engInst->GetDisplay()->screenWidth,
        engInst->GetDisplay()->screenHeight);
    tarLightAccumulation->CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM);
    GBuffer.insert(tarLightAccumulation->GetRenderTargetView());
    GBufferSRV.insert(tarLightAccumulation->GetRenderTargetSRV());
    GBufferViewports.insert(tarLightAccumulation->GetViewport());

    tarDiffuse = new RenderTarget(TargetViewType::Texture, engInst->GetDisplay()->screenWidth, engInst->GetDisplay()->screenHeight);
    tarDiffuse->CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM);
    GBuffer.insert(tarDiffuse->GetRenderTargetView());
    GBufferSRV.insert(tarDiffuse->GetRenderTargetSRV());
    GBufferViewports.insert(tarDiffuse->GetViewport());

    tarSpecular = new RenderTarget(TargetViewType::Texture, engInst->GetDisplay()->screenWidth, engInst->GetDisplay()->screenHeight);
    tarSpecular->CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM);
    GBuffer.insert(tarSpecular->GetRenderTargetView());
    GBufferSRV.insert(tarSpecular->GetRenderTargetSRV());
    GBufferViewports.insert(tarSpecular->GetViewport());

    tarNormal = new RenderTarget(TargetViewType::Texture, engInst->GetDisplay()->screenWidth, engInst->GetDisplay()->screenHeight);
    tarNormal->CreateRenderTarget();
    GBuffer.insert(tarNormal->GetRenderTargetView());
    GBufferSRV.insert(tarNormal->GetRenderTargetSRV());
    GBufferViewports.insert(tarNormal->GetViewport());

    GBufferSRV.insert(tarDepthStencil->GetDepthStencilSRV());
    GBufferViewports.insert(tarDepthStencil->GetViewport());

    CreateConstantBuffers();
    CreateShader();
    CreateVertexes();
    CreateBlendState();
    CreateRasterizerState();
}

void DeferredLightTechnique::Destroy()
{

    if (tarDepthStencil) tarDepthStencil->Destroy();
    if (tarLightAccumulation) tarLightAccumulation->Destroy();
    if (tarDiffuse) tarDiffuse->Destroy();
    if (tarSpecular) tarSpecular->Destroy();
    if (tarNormal) tarNormal->Destroy();

    if (lightIndexBuf) lightIndexBuf->Destroy();
    if (screenToWorldBuf) screenToWorldBuf->Destroy();

    if (deferredLightShader) deferredLightShader->Destroy();
}

void DeferredLightTechnique::GBufferPass()
{
    tarDepthStencil->ClearDepthStencil();
    tarLightAccumulation->ClearTarget();
    tarDiffuse->ClearTarget();
    tarSpecular->ClearTarget();
    tarNormal->ClearTarget();
    engInst->GetContext()->OMSetRenderTargets(4, &GBuffer[0], tarDepthStencil->GetDepthStencilView());
    engInst->GetContext()->RSSetViewports(GBufferViewports.size(), &GBufferViewports[0]);
    engInst->SetRenderState(RenderState::Deferred_GBuffer);
    engInst->RenderScene();
}

void DeferredLightTechnique::LightingPass()
{
    engInst->SetRenderState(RenderState::Deferred_Lighting);
    engInst->GetTexRenderTarget()->BindTarget(false);
    engInst->GetTexRenderTarget()->SetDepthStencilState();
    engInst->GetTexRenderTarget()->Clear();
    float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    UINT sampleMask = 0xffffffff;
    auto lights = engInst->GetLightComponents();
    for (int32_t i = 0; i < lights.size(); ++i)
    {
        if (!lights[i]->GetLightData().enabled) continue;
        PreRenderLightPassByLightID(i);
        engInst->GetContext()->OMSetBlendState(blendState, blendFactor, sampleMask);
        engInst->GetContext()->RSSetState(nullptr);
        //engInst->GetContext()->RSSetState(rastState);
        const uint32_t stride = sizeof(PostProcessVertex);
        const uint32_t offset = 0;
        engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        deferredLightShader->Draw();
        engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
        //engInst->GetContext()->DrawAuto();
    }
}

void DeferredLightTechnique::PreRenderLightPassByLightID(int32_t lightId)
{
    engInst->GetContext()->PSSetShaderResources(0, 5, &GBufferSRV[0]);
    const LightIndexBuffer id{lightId};
    lightIndexBuf->UpdateData(id);
    lightIndexBuf->SetBuffer(4, SPixel);

    const auto cam = engInst->GetCurCamera();
    const auto invertProjView = cam->GetEyeData().GetViewProj().Invert();
    const auto camPos = cam->GetSceneComponent()->GetWorldLocation();
    const ScreenToWorldParams ScreenToWorld{invertProjView.Transpose(),
                                            camPos,
                                            Vector2(static_cast<float>(engInst->GetDisplay()->screenWidth),
                                                static_cast<float>(engInst->GetDisplay()->screenHeight))};
    screenToWorldBuf->UpdateData(ScreenToWorld);
    screenToWorldBuf->SetBuffer(2, SPixel);

    engInst->BindLightsBuffer();
}

void DeferredLightTechnique::CreateConstantBuffers()
{
    lightIndexBuf = (new Buffer<LightIndexBuffer>())->CreateBuffer();
    screenToWorldBuf = (new Buffer<ScreenToWorldParams>())->CreateBuffer();
}

void DeferredLightTechnique::CreateShader()
{
    deferredLightShader = new Shader();
    deferredLightShader->AddInputElementDesc("POSITION");
    deferredLightShader->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
    std::strstream s;
    std::string numLights;
    s << engInst->GetLightComponents().size() << "\x00";
    s >> numLights;
    D3D_SHADER_MACRO macro[] = {"NUM_LIGHTS", numLights.c_str(), nullptr,
                                nullptr};
    deferredLightShader->CreateShader(L"./Resource/Shaders/DeferredLightShader.hlsl", SVertex, macro);
    deferredLightShader->CreateShader(L"./Resource/Shaders/DeferredLightShader.hlsl", SPixel, macro);
}

void DeferredLightTechnique::CreateVertexes()
{
    signed int centreW = engInst->GetDisplay()->screenWidth / 2 * -1;
    signed int centreH = engInst->GetDisplay()->screenHeight / 2;
    float left = (float)centreW;
    float right = left + engInst->GetDisplay()->screenWidth;
    float top = (float)centreH;
    float bottom = top - engInst->GetDisplay()->screenHeight;

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
    CreateVertexBuffer();
    CreateIndexBuffer();
}

void DeferredLightTechnique::CreateVertexBuffer()
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

void DeferredLightTechnique::CreateIndexBuffer()
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

void DeferredLightTechnique::CreateBlendState()
{
    D3D11_BLEND_DESC blendStateDesc = {};
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    engInst->GetDevice()->CreateBlendState(&blendStateDesc, &blendState);
}

void DeferredLightTechnique::CreateRasterizerState()
{
}
