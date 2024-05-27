#include "DeferredLightTechnique.h"

#include <strstream>

#include "BlendState.h"
#include "RenderTarget.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"
#include "Core/Components/SceneComponent.h"
#include "Utils/DebugDrawing.h"
#include "Core/Render/Buffer.h"

DeferredLightTechnique::DeferredLightTechnique()
{
    engInst = &Engine::GetInstance();
}

void DeferredLightTechnique::Render()
{
    GBufferPass();
    LightingPass();
    engInst->GetTexRenderTarget()->CopyDepthStencilViewTex(tarDepthStencil->GetDepthStencilTexture());
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
    CreateShaders();
    CreateVertices();
    CreateBlendState();
    CreateRasterizerState();
    CreateDepthStencilStates();
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

    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (blendState) blendState->Destroy();

    if (rastStateCullFront) rastStateCullFront->Release();
    if (rastStateCullBack) rastStateCullBack->Release();

    if (quadShader) quadShader->Destroy();
    if (fullQuadShader) fullQuadShader->Destroy();
    if (volumeShader) volumeShader->Destroy();
    if (allQuadShader) allQuadShader->Destroy();
    if (allVolumeShader) allVolumeShader->Destroy();
    if (renderOneLightTypeShader) renderOneLightTypeShader->Destroy();

    if (DSSGreater) DSSGreater->Release();
    if (DSSLess) DSSLess->Release();
}

void DeferredLightTechnique::GBufferPass()
{
    engInst->SetRenderState(RenderState::Deferred_GBuffer);
    tarDepthStencil->ClearDepthStencil();
    tarLightAccumulation->ClearTarget();
    tarDiffuse->ClearTarget();
    tarSpecular->ClearTarget();
    tarNormal->ClearTarget();
    engInst->GetContext()->OMSetRenderTargets(4, &GBuffer[0], tarDepthStencil->GetDepthStencilView());
    engInst->GetContext()->RSSetViewports(GBufferViewports.size(), &GBufferViewports[0]);
    engInst->RenderScene();
}

void DeferredLightTechnique::LightingPass()
{
    engInst->SetRenderState(RenderState::Deferred_Lighting);
    engInst->GetTexRenderTarget()->Clear();

    //engInst->GetContext()->RSSetState(rastStateCullBack);
    engInst->GetTexRenderTarget()->SetDepthStencilState(DSSLess);
    blendState->Bind();
    engInst->GetTexRenderTarget()->BindTarget();
    const auto SRV = tarLightAccumulation->GetRenderTargetSRV();
    engInst->GetContext()->PSSetShaderResources(0, 1, &SRV);
    DrawDirectionalLightVolume(fullQuadShader);

    SortLights();
    engInst->GetTexRenderTarget()->BindTarget();

    /*auto camData =    engInst->GetCurEyeData();;
    camData.isCam = true;
    engInst->SetCurEyeData(camData);*/
    blendState->Bind();
    DrawLightsByType(LightType::DirectionalLight);
    DrawLightsByType(LightType::PointLight);
    DrawLightsByType(LightType::SpotLight);

    /*for (int32_t i = 0; i < lights.size(); ++i)
    {
        if (!lights[i]->GetLightData().enabled) continue;
        blendState->UnBind();

        engInst->GetContext()->RSSetState(rastStateCullBack);
        engInst->GetTexRenderTarget()->SetDepthStencilState(DSSLess);
        engInst->GetTexRenderTarget()->BindDepthStencil();
        lights[i]->Render();
        if (lights[i]->GetLightData().type == DIRECTIONAL_LIGHT) DrawDirectionalLightVolume(quadShader);
        else DrawLightVolume(lights[i], volumeShader);

        engInst->GetContext()->RSSetState(rastStateCullFront);
        engInst->GetTexRenderTarget()->SetDepthStencilState(DSSGreater);
        engInst->GetTexRenderTarget()->BindDepthStencil();
        lights[i]->Render();
        if (lights[i]->GetLightData().type == DIRECTIONAL_LIGHT) DrawDirectionalLightVolume(quadShader);
        else DrawLightVolume(lights[i], volumeShader);

        engInst->GetContext()->RSSetState(rastStateCullBack);
        engInst->GetTexRenderTarget()->SetDepthStencilState(DSSLess);
        engInst->GetTexRenderTarget()->BindTarget();
        blendState->Bind();
        lights[i]->Render();
        PreRenderLightPassByLightID(i);
        if (lights[i]->GetLightData().type == DIRECTIONAL_LIGHT) DrawDirectionalLightVolume(allQuadShader);
        else DrawLightVolume(lights[i], allVolumeShader);
        
    }*/
    engInst->GetContext()->RSSetState(nullptr);
    blendState->UnBind();
}

void DeferredLightTechnique::DrawLightVolume(LightComponent* light, Shader* curShader)
{
    ID3D11Buffer* vertBuf = nullptr;
    ID3D11Buffer* indBuf = nullptr;
    Array<VertexNoTex> verts;
    Array<int32_t> idxs;
    light->GetLightVolume(verts, idxs);
    light->GetLightVolumeBuffers(&vertBuf, &indBuf);

    const uint32_t stride = sizeof(VertexNoTex);
    const uint32_t offset = 0;
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertBuf, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indBuf, DXGI_FORMAT_R32_UINT, 0);
    curShader->BindShaders();
    engInst->GetContext()->DrawIndexed(idxs.size(), 0, 0);

}

void DeferredLightTechnique::DrawDirectionalLightVolume(Shader* curShader)
{
    const uint32_t stride = sizeof(VertexNoTex);
    const uint32_t offset = 0;
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    curShader->BindShaders();
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void DeferredLightTechnique::DrawLightsByType(LightType type)
{
    Array<LightComponent*> lights;
    if (type == LightType::PointLight) lights = pointLights;
    if (type == LightType::SpotLight) lights = spotLights;
    if (type == LightType::DirectionalLight) lights = directionalLights;
    PreRenderLightPassByLights(lights);
    const uint32_t stride[2]  = {sizeof(VertexNoTex), 0};
    const uint32_t offset[2] = {0, 0};
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, stride, offset);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    renderOneLightTypeShader->BindShaders();
    engInst->GetContext()->DrawIndexedInstanced(indexes.size(), lights.size(), 0, 0, 0);
}

void DeferredLightTechnique::PreRenderLightPassByLightID(int32_t lightId)
{
    engInst->GetContext()->PSSetShaderResources(0, 5, &GBufferSRV[0]);
    const LightIndexBuffer id{lightId};
    lightIndexBuf->UpdateData(&id);
    lightIndexBuf->BindBuffers(4, SPixel);

    BindScreenToWorldData();

    engInst->BindLightsBuffer();
}

void DeferredLightTechnique::PreRenderLightPassByLights(const Array<LightComponent*>& lights)
{
    engInst->GetContext()->PSSetShaderResources(0, 5, &GBufferSRV[0]);

    BindScreenToWorldData();
    
    curLightsData.clear();
    for (const auto lightComp : lights)
    {
        curLightsData.insert(lightComp->GetLightData());
    }
    
    for (const auto light : lights)
    {
        light->UpdateLightData();
        if (light->GetLightData().type == LightType::DirectionalLight)
            light->BindShadowMapSRV();
    }
    
    const auto lightsSRV = engInst->GetLightsStructuredSRV();
    const auto lightsBuf = engInst->GetLightsBuffer();
    
    lightsBuf->UpdateData(&curLightsData[0]);
    engInst->GetContext()->PSSetShaderResources(8, 1, &lightsSRV);
}

void DeferredLightTechnique::SortLights()
{
    auto lights = engInst->GetLightComponents();
    pointLights.clear();
    spotLights.clear();
    directionalLights.clear();
    for (const auto light : lights)
    {
        if (!light->GetLightData().enabled) continue;
        if (light->GetLightData().type == LightType::PointLight)
            pointLights.insert(light);
        if (light->GetLightData().type == LightType::SpotLight)
            spotLights.insert(light);
        if (light->GetLightData().type == LightType::DirectionalLight)
            directionalLights.insert(light);
    }
}


void DeferredLightTechnique::BindScreenToWorldData()
{
    const auto cam = engInst->GetCurCamera();
    const auto invertProjView = cam->GetEyeData().GetViewProj().Invert();
    const auto camPos = cam->GetSceneComponent()->GetWorldLocation();
    const ScreenToWorldParams ScreenToWorld{invertProjView.Transpose(),
                                            cam->GetEyeData().GetViewProj().Transpose(),
                                            camPos,
                                            Vector2(static_cast<float>(engInst->GetDisplay()->screenWidth),
                                                static_cast<float>(engInst->GetDisplay()->screenHeight))};
    screenToWorldBuf->UpdateData(&ScreenToWorld);
    screenToWorldBuf->BindBuffers(2, SPixel);
}

void DeferredLightTechnique::CreateConstantBuffers()
{
    lightIndexBuf = (new Buffer())->CreateBuffer<LightIndexBuffer>();
    screenToWorldBuf = (new Buffer())->CreateBuffer<ScreenToWorldParams>();
}

void DeferredLightTechnique::CreateShaders()
{
    std::strstream s;
    std::string cascade, numLights;
    s << engInst->GetLightComponents().size() << "\x00";
    s >> numLights;
    s.clear();
    s << CASCADE_COUNT << "\x00";
    s >> cascade;
    D3D_SHADER_MACRO macro[] = {"DO_CASCADE_SHADOW", "1", "CASCADE_COUNT", cascade.c_str(), "NUM_LIGHTS", numLights.c_str(), nullptr,
                                nullptr};

    quadShader = new Shader();
    quadShader->AddInputElementDesc("POSITION");
    quadShader->AddInputElementDesc("COLOR");
    quadShader->CreateShader(L"./Resource/Shaders/FullQuadShader.hlsl", SVertex, macro);

    fullQuadShader = new Shader();
    fullQuadShader->AddInputElementDesc("POSITION");
    fullQuadShader->AddInputElementDesc("COLOR");
    fullQuadShader->CreateShader(L"./Resource/Shaders/FullQuadShader.hlsl", SVertex, macro);
    fullQuadShader->CreateShader(L"./Resource/Shaders/FullQuadShader.hlsl", SPixel, macro);

    allQuadShader = new Shader();
    allQuadShader->AddInputElementDesc("POSITION");
    allQuadShader->AddInputElementDesc("COLOR");
    allQuadShader->CreateShader(L"./Resource/Shaders/FullQuadShader.hlsl", SVertex, macro);
    allQuadShader->CreateShader(L"./Resource/Shaders/DeferredLightShader.hlsl", SPixel, macro);

    volumeShader = new Shader();
    volumeShader->AddInputElementDesc("POSITION");
    volumeShader->AddInputElementDesc("COLOR");
    volumeShader->CreateShader(L"./Resource/Shaders/DeferredLightShader.hlsl", SVertex, macro);

    allVolumeShader = new Shader();
    allVolumeShader->AddInputElementDesc("POSITION");
    allVolumeShader->AddInputElementDesc("COLOR");
    allVolumeShader->CreateShader(L"./Resource/Shaders/FullQuadShader.hlsl", SVertex, macro);
    allVolumeShader->CreateShader(L"./Resource/Shaders/DeferredLightShader.hlsl", SPixel, macro);

    renderOneLightTypeShader = new Shader();
    renderOneLightTypeShader->CreateShader(L"./Resource/Shaders/DeferredAllLightsShader.hlsl", SVertex, macro);
    renderOneLightTypeShader->CreateShader(L"./Resource/Shaders/DeferredAllLightsShader.hlsl", SPixel, macro);
}

void DeferredLightTechnique::CreateVertices()
{
    DebugDrawing::CreateFullQuad(Vector2((float)engInst->GetDisplay()->screenWidth, (float)engInst->GetDisplay()->screenHeight), vertices,
        indexes);
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
    vertexBufDesc.ByteWidth = sizeof(VertexNoTex) * vertices.size();

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
    blendState = (new BlendState())->CreateAdditive();
}

void DeferredLightTechnique::CreateDepthStencilStates()
{
    CD3D11_DEPTH_STENCIL_DESC descDSS(D3D11_DEFAULT);
    descDSS.DepthEnable = true;
    descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    descDSS.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    descDSS.StencilEnable = true;
    descDSS.StencilReadMask = 1;
    descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    engInst->GetDevice()->CreateDepthStencilState(&descDSS, &DSSGreater);

    descDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    descDSS.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    engInst->GetDevice()->CreateDepthStencilState(&descDSS, &DSSLess);
}

void DeferredLightTechnique::CreateRasterizerState()
{
    CD3D11_RASTERIZER_DESC rastDesc(D3D11_DEFAULT);
    rastDesc.CullMode = D3D11_CULL_FRONT;
    engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastStateCullFront);

    rastDesc.CullMode = D3D11_CULL_BACK;
    engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastStateCullBack);
}
