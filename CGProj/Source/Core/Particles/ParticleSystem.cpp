#include "ParticleSystem.h"

#include "Core/Components/TextureComponent.h"
#include "Core/Render/BlendState.h"
#include "Core/Render/Buffer.h"
#include "Core/Render/RenderTarget.h"
#include "Core/Render/Shader.h"
#include "Utils/HelperFunctions.h"

ParticleSystem::ParticleSystem(int32_t particleCount, const Transform& transform, SceneComponent* parentComp)
    : particleCount(particleCount)
{
    sceneComp = CreateComponent<SceneComponent>(transform, parentComp);
    textureComp = CreateComponent<TextureComponent>();
}

void ParticleSystem::Destroy()
{
    if (particleShader) particleShader->Destroy();
    if (particleComputeShader) particleComputeShader->Destroy();
    if (particlesBuf) particlesBuf->Destroy();
    if (handlerBuf) handlerBuf->Destroy();
    if (textureComp) textureComp->DestroyResource();
    if (particlesSRV) particlesSRV->Release();
    if (particlesUAV) particlesUAV->Release();
    if (blendState) blendState->Destroy();
    if (sceneComp) sceneComp->DestroyResource();
    Object::Destroy();
}

void ParticleSystem::Initialize()
{
    CreateShader();
    CreateBuffer();

    int32_t numGroups = (particleCount % 768 != 0) ? ((particleCount / 768) + 1) : (particleCount / 768);
    double secondRoot = pow((double)numGroups, (double)(1.0 / 2.0));
    secondRoot = ceill(secondRoot);
    groupSizeX = groupSizeY = (int32_t)secondRoot;

    blendState = (new BlendState())->CreateAdditive();
    Object::Initialize();
}

void ParticleSystem::Update(float timeTick)
{
    Object::Update(timeTick);
    HandlerData data;
    data.DeltaTime = timeTick;
    data.TotalTime = engInst->GetTotalTime();
    data.MaxParticles = particleCount;
    data.GroupDim = groupSizeX;
    handlerBuf->UpdateData(&data);

    uint32_t initCount = -1;
    engInst->GetContext()->CSSetUnorderedAccessViews(0, 1, &particlesUAV, &initCount);
    handlerBuf->BindBuffers(0, SCompute);
    particleComputeShader->BindShaders();
    engInst->GetContext()->Dispatch(groupSizeX, groupSizeY, 1);
}

void ParticleSystem::Draw()
{
    //engInst->GetTexRenderTarget()->BindTarget();
    engInst->GetContext()->RSSetState(nullptr);
    blendState->Bind();
    Object::Draw();
    particleShader->BindShaders();
    engInst->GetContext()->VSSetShaderResources(0, 1, &particlesSRV);
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    engInst->GetContext()->Draw(particleCount, 0);

    blendState->UnBind();
}

void ParticleSystem::CreateShader()
{
    particleShader = new Shader();
    //particleShader->AddInputElementDesc();
    particleShader->CreateShader(L"./Resource/Shaders/ParticleShader.hlsl", SVertex);
    particleShader->CreateShader(L"./Resource/Shaders/ParticleShader.hlsl", SGeometry);
    particleShader->CreateShader(L"./Resource/Shaders/ParticleShader.hlsl", SPixel);

    particleComputeShader = new Shader();
    particleComputeShader->CreateShader(L"./Resource/Shaders/ParticleShaderCompute.hlsl", SCompute);
}

void ParticleSystem::CreateBuffer()
{
    particlesBuf = (new Buffer())->CreateBuffer<ParticleData>(D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
        0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(ParticleData) * particleCount, nullptr, sizeof(ParticleData));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = particleCount;
    engInst->GetDevice()->CreateShaderResourceView(particlesBuf->GetBuffer(), &srvDesc, &particlesSRV);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = particleCount;
    uavDesc.Buffer.Flags = 0;
    engInst->GetDevice()->CreateUnorderedAccessView(particlesBuf->GetBuffer(), &uavDesc, &particlesUAV);

    ParticleData* initParticles = new ParticleData[particleCount];
    for (int32_t i = 0; i < particleCount; ++i)
    {
        initParticles[i].pos = Rand(-Vector3::One * 50.0f, Vector3::One * 50.0f);
    }
    particlesBuf->UpdateData(initParticles);

    handlerBuf = (new Buffer())->CreateBuffer<HandlerData>();
    delete [] initParticles;
}
