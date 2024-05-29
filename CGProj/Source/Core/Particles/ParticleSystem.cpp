#include "ParticleSystem.h"

#include "SortLib.h"
#include "Core/Windisplay.h"
#include "Core/Components/TextureComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Render/BlendState.h"
#include "Core/Render/Buffer.h"
#include "Core/Render/DeferredLightTechnique.h"
#include "Core/Render/RenderTarget.h"
#include "Core/Render/Shader.h"
#include "Utils/HelperFunctions.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

int align(int value, int alignment) { return (value + (alignment - 1)) & ~(alignment - 1); }

ParticleSystem::ParticleSystem(int32_t maxParticleCount, const Transform& transform, SceneComponent* parentComp)
    : maxParticleCount(maxParticleCount)
{
    sceneComp = CreateComponent<SceneComponent>(transform, parentComp);
    textureComp = CreateComponent<TextureComponent>();
}

void ParticleSystem::Destroy()
{
    if (sortLib) sortLib->Destroy();

    if (renderShader) renderShader->Destroy();
    if (emitShader) emitShader->Destroy();
    if (simulateShader) simulateShader->Destroy();
    if (resetShader) resetShader->Destroy();
    if (particleComputeShader) particleComputeShader->Destroy();
    if (initDeadListShader) initDeadListShader->Destroy();

    if (particlesBuffs) particlesBuffs->Destroy();
    if (handlerBuf) handlerBuf->Destroy();
    if (emitterBuf) emitterBuf->Destroy();
    if (deadListCountBuf) deadListCountBuf->Destroy();
    if (activeListCountBuf) activeListCountBuf->Destroy();

    if (textureComp) textureComp->DestroyResource();

    SAFE_RELEASE(particlesA_SRV);
    SAFE_RELEASE(particlesB_SRV);
    SAFE_RELEASE(viewSpaceParticlePosSRV);
    SAFE_RELEASE(maxRadiusSRV);
    SAFE_RELEASE(aliveIndexBufferSRV);

    SAFE_RELEASE(particlesA_UAV);
    SAFE_RELEASE(particlesB_UAV);
    SAFE_RELEASE(viewSpaceParticlePosUAV);
    SAFE_RELEASE(maxRadiusUAV);
    SAFE_RELEASE(aliveIndexBufferUAV);
    SAFE_RELEASE(deadListUAV);

    SAFE_RELEASE(randomTexture);
    SAFE_RELEASE(randomTextureSRV);
    if (blendState) blendState->Destroy();
    if (sceneComp) sceneComp->DestroyResource();
    Object::Destroy();
}

void ParticleSystem::Initialize()
{
    CreateShaders();
    CreateBuffers();
    CreateBlendState();
    CreateSamplers();
    FillRandomTexture();
    InitSort();
    if (emitter.startSize == 0.0f || emitter.endSize == 0.0f) InitDefaultEmitter();
    needReset = true;
    particleCount = maxParticleCount;
    particleEmitRate = 100;
    //textureComp->SetOpacity(0.5f);
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &ParticleSystem::OnKeyDown);
    Object::Initialize();
}

void ParticleSystem::Reload()
{
    particleCount = maxParticleCount;
    needReset = true;
    Object::Reload();
}

void ParticleSystem::Update(float timeTick)
{
    Object::Update(timeTick);
    const auto camera = engInst->GetCurCamera();
    const auto eyeData = camera->GetEyeData();
    const auto invertProjView = eyeData.GetViewProj().Invert();

    HandlerData data;
    data.mInvProjView = invertProjView.Transpose();
    data.mInvProj = eyeData.mProj.Invert().Transpose();
    data.mInvView = eyeData.mView.Invert().Transpose();
    data.mView = eyeData.mView.Transpose();
    data.mProj = eyeData.mProj.Transpose();
    data.mViewProj = eyeData.GetViewProj().Transpose();

    data.CamPos = camera->GetSceneComponent()->GetWorldLocation();
    data.MaxParticles = maxParticleCount;
    data.ScreenDimensions = Vector2(static_cast<float>(engInst->GetDisplay()->screenWidth),
        static_cast<float>(engInst->GetDisplay()->screenHeight));

    data.TotalTime = engInst->GetTotalTime();
    data.DeltaTime = timeTick;

    data.CollisionThickness = 0.1f;
    data.CollideParticles = doCollide ? 1 : 0;
    data.ShowSleepingParticles = 0;
    data.EnableSleepState = 0;

    handlerBuf->UpdateData(&data);
}

void ParticleSystem::PreDraw()
{
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = nullptr;
    engInst->GetContext()->OMGetRenderTargets(1, &rtv, &dsv);
    handlerBuf->BindBuffers(0, SCompute | SPixel | SVertex | SGeometry);

    engInst->GetContext()->PSSetSamplers(0, 1, &samWrapLinear);
    engInst->GetContext()->PSSetSamplers(1, 1, &samClampLinear);

    engInst->GetContext()->CSSetSamplers(0, 1, &samWrapLinear);
    engInst->GetContext()->CSSetSamplers(1, 1, &samClampLinear);

    engInst->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

    if (needReset)
    {
        InitDeadList();
        ID3D11UnorderedAccessView* uavs[] = {particlesA_UAV, particlesB_UAV};
        UINT initialCounts[] = {(UINT)-1, (UINT)-1};

        engInst->GetContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);

        resetShader->BindShaders();
        engInst->GetContext()->Dispatch(align(maxParticleCount, 256) / 256, 1, 1);

        needReset = false;
    }

    curParticleToEmit = particleEmitRate % particleCount;
    particleCount -= curParticleToEmit;
    Emit(curParticleToEmit);
    const auto depthSRV = engInst->GetDeferredTech()->GetDepthStencilSRV();
    Simulate(depthSRV, true);

    engInst->GetContext()->CopyStructureCount(activeListCountBuf->GetBuffer(), 0, aliveIndexBufferUAV);

    Sort();

    engInst->GetContext()->OMSetRenderTargets(1, &rtv, dsv);
    SAFE_RELEASE(rtv);
    SAFE_RELEASE(dsv);

    Object::PreDraw();
}

void ParticleSystem::Draw()
{
    engInst->GetContext()->RSSetState(nullptr);
    blendState->Bind();
    Object::Draw();

    handlerBuf->BindBuffers(0, SCompute | SPixel | SVertex | SGeometry);
    renderShader->BindShaders();

    ID3D11Buffer* vb = nullptr;
    UINT stride = 0;
    UINT offset = 0;
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    activeListCountBuf->BindBuffers(3, SVertex);

    ID3D11ShaderResourceView* vs_srv[] = {particlesA_SRV, viewSpaceParticlePosSRV, aliveIndexBufferSRV};
    const auto depthSRV = engInst->GetDeferredTech()->GetDepthStencilSRV();
    ID3D11ShaderResourceView* ps_srv[] = {depthSRV};
    engInst->GetContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
    engInst->GetContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);

    engInst->GetContext()->DrawInstancedIndirect(indirectDrawArgsBuf->GetBuffer(), 0);

    ZeroMemory(vs_srv, sizeof( vs_srv ));
    engInst->GetContext()->VSSetShaderResources(0, ARRAYSIZE(vs_srv), vs_srv);
    ZeroMemory(ps_srv, sizeof( ps_srv ));
    engInst->GetContext()->PSSetShaderResources(1, ARRAYSIZE(ps_srv), ps_srv);
    blendState->UnBind();
}

void ParticleSystem::Emit(int32_t numToEmit)
{
    ID3D11UnorderedAccessView* uavs[] = {particlesA_UAV, particlesB_UAV, deadListUAV};
    UINT initialCounts[] = {(UINT)-1, (UINT)-1, (UINT)-1};
    engInst->GetContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);

    ID3D11Buffer* buffers[] = {emitterBuf->GetBuffer(), deadListCountBuf->GetBuffer()};
    engInst->GetContext()->CSSetConstantBuffers(1, ARRAYSIZE(buffers), buffers);

    ID3D11ShaderResourceView* srvs[] = {randomTextureSRV};
    engInst->GetContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

    emitShader->BindShaders();

    // Run CS for each emitter

    if (numToEmit > 0)
    {
        emitter.maxParticlesThisFrame = numToEmit;
        emitter.ID = 0;
        emitter.streaks = emitter.streaks ? 1 : 0;
        //engInst->GetContext()->Unmap(emitterBuf->GetBuffer(), 0);
        emitterBuf->UpdateData(&emitter);

        // Copy the current number of dead particles into a CB so we know how many new particles are available to be spawned
        engInst->GetContext()->CopyStructureCount(deadListCountBuf->GetBuffer(), 0, deadListUAV);

        // Dispatch enough thread groups to spawn the requested particles
        int numThreadGroups = align(numToEmit, 1024) / 1024;
        engInst->GetContext()->Dispatch(numThreadGroups, 1, 1);
    }

}

void ParticleSystem::Simulate(ID3D11ShaderResourceView* depthSRV, bool useGS)
{
    // Set the UAVs and reset the alive index buffer's counter
    ID3D11UnorderedAccessView* uavs[] = {particlesA_UAV, particlesB_UAV, deadListUAV, aliveIndexBufferUAV, viewSpaceParticlePosUAV,
                                         maxRadiusUAV, indirectDrawArgsUAV};
    UINT initialCounts[] = {(UINT)-1, (UINT)-1, (UINT)-1, 0, (UINT)-1, (UINT)-1, (UINT)-1};

    engInst->GetContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);

    // Bind the depth buffer as a texture for doing collision detection and response
    ID3D11ShaderResourceView* srvs[] = {depthSRV};
    engInst->GetContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

    // Pick the correct CS based on the system's options
    //BillboardMode billboardMode = flags & PF_UseGeometryShader ? UseGS : UseVS;

    // Dispatch enough thread groups to update all the particles
    //engInst->GetContext()->CSSetShader( m_pCSSimulate[ billboardMode ], nullptr, 0 );
    if (useGS) simulateShader->BindShaders();
    engInst->GetContext()->Dispatch(align(maxParticleCount, 256) / 256, 1, 1);

    ZeroMemory(srvs, sizeof( srvs ));
    engInst->GetContext()->CSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
    ZeroMemory(uavs, sizeof( uavs ));
    engInst->GetContext()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
}

void ParticleSystem::Sort()
{
    sortLib->Sort(maxParticleCount, aliveIndexBufferUAV, activeListCountBuf->GetBuffer());
}

void ParticleSystem::FillRandomTexture()
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof( desc ));
    desc.Width = 1024;
    desc.Height = 1024;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    float* values = new float[desc.Width * desc.Height * 4];
    float* ptr = values;
    for (UINT i = 0; i < desc.Width * desc.Height; i++)
    {
        ptr[0] = RandomVariance(0.0f, 1.0f);
        ptr[1] = RandomVariance(0.0f, 1.0f);
        ptr[2] = RandomVariance(0.0f, 1.0f);
        ptr[3] = RandomVariance(0.0f, 1.0f);
        ptr += 4;
    }

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = values;
    data.SysMemPitch = desc.Width * 16;
    data.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateTexture2D(&desc, &data, &randomTexture);

    delete[] values;

    D3D11_SHADER_RESOURCE_VIEW_DESC srv;
    srv.Format = desc.Format;
    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MipLevels = 1;
    srv.Texture2D.MostDetailedMip = 0;

    engInst->GetDevice()->CreateShaderResourceView(randomTexture, &srv, &randomTextureSRV);
}

void ParticleSystem::InitDeadList()
{
    initDeadListShader->BindShaders();

    UINT initialCount[] = {0};
    engInst->GetContext()->CSSetUnorderedAccessViews(0, 1, &deadListUAV, initialCount);

    // Disaptch a set of 1d thread groups to fill out the dead list, one thread per particle
    engInst->GetContext()->Dispatch(align(maxParticleCount, 256) / 256, 1, 1);

}

void ParticleSystem::InitSort()
{
    sortLib = new SortLib();
    sortLib->InitSort();
}

void ParticleSystem::CreateShaders()
{
    renderShader = new Shader();
    //particleShader->AddInputElementDesc();
    renderShader->CreateShader(L"./Resource/Shaders/Particles/RenderShader.hlsl", SVertex);
    renderShader->CreateShader(L"./Resource/Shaders/Particles/RenderShader.hlsl", SGeometry);
    renderShader->CreateShader(L"./Resource/Shaders/Particles/RenderShader.hlsl", SPixel);

    particleComputeShader = new Shader();
    particleComputeShader->CreateShader(L"./Resource/Shaders/Particles/ParticleComputeShader.hlsl", SCompute);

    emitShader = new Shader();
    emitShader->CreateShader(L"./Resource/Shaders/Particles/EmitShader.hlsl", SCompute, nullptr, (LPSTR)"CS_Emit");

    simulateShader = new Shader();
    simulateShader->CreateShader(L"./Resource/Shaders/Particles/SimulateShader.hlsl", SCompute, nullptr, (LPSTR)"CS_Simulate");

    resetShader = new Shader();
    resetShader->CreateShader(L"./Resource/Shaders/Particles/SimulateShader.hlsl", SCompute, nullptr, (LPSTR)"CS_Reset");

    initDeadListShader = new Shader();
    initDeadListShader->CreateShader(L"./Resource/Shaders/Particles/InitDeadListShader.hlsl", SCompute, nullptr,
        (LPSTR)"CS_InitDeadList");
}

void ParticleSystem::CreateBuffers()
{
    //PartA : 0
    particlesBuffs = (new Buffer())->CreateBuffer<ParticleBuf>(D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(ParticleBuf) * maxParticleCount, nullptr, sizeof(ParticleBuf));
    //PartB : 1
    particlesBuffs->CreateBuffer<ParticleBuf>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(ParticleBuf) * maxParticleCount, nullptr, sizeof(ParticleBuf));
    //ViewSpacePos : 2
    particlesBuffs->CreateBuffer<Vector4>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(Vector4) * maxParticleCount, nullptr, sizeof(Vector4));
    //MaxRadius : 3
    particlesBuffs->CreateBuffer<float>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * maxParticleCount, nullptr, sizeof(float));
    //AliveIndex : 4
    particlesBuffs->CreateBuffer<IndexBufferElement>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(IndexBufferElement) * maxParticleCount, nullptr, sizeof(IndexBufferElement));
    //DeadList : 5
    particlesBuffs->CreateBuffer<uint32_t>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * maxParticleCount, nullptr, sizeof(uint32_t));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = maxParticleCount;
    engInst->GetDevice()->CreateShaderResourceView(particlesBuffs->GetBuffer(0), &srvDesc, &particlesA_SRV);
    engInst->GetDevice()->CreateShaderResourceView(particlesBuffs->GetBuffer(1), &srvDesc, &particlesB_SRV);
    engInst->GetDevice()->CreateShaderResourceView(particlesBuffs->GetBuffer(2), &srvDesc, &viewSpaceParticlePosSRV);
    engInst->GetDevice()->CreateShaderResourceView(particlesBuffs->GetBuffer(3), &srvDesc, &maxRadiusSRV);
    engInst->GetDevice()->CreateShaderResourceView(particlesBuffs->GetBuffer(4), &srvDesc, &aliveIndexBufferSRV);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = maxParticleCount;
    uavDesc.Buffer.Flags = 0;
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(0), &uavDesc, &particlesA_UAV);
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(1), &uavDesc, &particlesB_UAV);
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(2), &uavDesc, &viewSpaceParticlePosUAV);
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(3), &uavDesc, &maxRadiusUAV);

    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(4), &uavDesc, &aliveIndexBufferUAV);

    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuffs->GetBuffer(5), &uavDesc, &deadListUAV);

    //INDIRECT
    indirectDrawArgsBuf = (new Buffer)->CreateBuffer<uint32_t>(D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, 0,
        D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS, 5 * sizeof(uint32_t));

    uavDesc.Format = DXGI_FORMAT_R32_UINT;
    uavDesc.Buffer.NumElements = 5;
    uavDesc.Buffer.Flags = 0;
    engInst->GetDevice()->CreateUnorderedAccessView(indirectDrawArgsBuf->GetBuffer(), &uavDesc, &indirectDrawArgsUAV);

    /*ParticleData* initParticles = new ParticleData[maxParticleCount];
    for (int32_t i = 0; i < maxParticleCount; ++i)
    {
        initParticles[i].pos = Rand(-Vector3::One * 50.0f, Vector3::One * 50.0f);
    }
    particlesBuf->UpdateData(initParticles);
    delete [] initParticles;*/

    handlerBuf = (new Buffer())->CreateBuffer<HandlerData>();
    emitterBuf = (new Buffer())->CreateBuffer<EmitterData>();
    deadListCountBuf = (new Buffer)->CreateBuffer<ListCount>();
    activeListCountBuf = (new Buffer)->CreateBuffer<ListCount>();
}

void ParticleSystem::CreateBlendState()
{
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //blendState = (new BlendState())->CreateBlendState(&blendDesc);
    blendState = (new BlendState())->CreateAdditive();
}

void ParticleSystem::CreateSamplers()
{
    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory(&samDesc, sizeof(samDesc));
    samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.MaxAnisotropy = 1;
    samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD = D3D11_FLOAT32_MAX;
    engInst->GetDevice()->CreateSamplerState(&samDesc, &samWrapLinear);

    //samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    engInst->GetDevice()->CreateSamplerState(&samDesc, &samClampLinear);
}

void ParticleSystem::InitDefaultEmitter()
{
    emitter.posVar = Vector4::One * 50.f;
    emitter.vel = Vector4(0.5f, 0.0f, 0.5f, 1.0f);
    emitter.velVar = 1.0f;
    emitter.lifeSpan = -1.0f;
    emitter.startSize = emitter.endSize = 0.1f;
    emitter.mass = 0.01f;
    emitter.ID = 0;
    emitter.streaks = 1;
}

void ParticleSystem::OnKeyDown(Keys key)
{
    if (key == Keys::L) doCollide = !doCollide;
}
