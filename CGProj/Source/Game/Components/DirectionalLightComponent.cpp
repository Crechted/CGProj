#include "DirectionalLightComponent.h"

#include "Core/Engine.h"
#include "Utils/Types.h"


void DirectionalLightComponent::Initialize()
{
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(DirectionLightData);
    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &lightBuffer);

    lightData.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    transform.rotate = Vector3(-45.0f, 45.0f, 00.0f);
    const auto forward = transform.GetForward();
    lightData.direction = Vector4(forward.x, forward.y, forward.z, 0.0f);
    lightData.direction.Normalize();
    lightData.kaSpecPowKsX = Vector4{ambietKoeff, specPow, specKoeff, 0.0f};

    Vector3 tar = transform.GetMatrix().Translation()+transform.GetForward();
    eyeData.mView = Matrix::CreateLookAt(transform.location, tar, transform.GetUp());

    const float screenNear = -50.0f;
    const float screenDepth = 50.0f;
    
    eyeData.mProj = Matrix::CreateOrthographic(50.0f, 50.0f, screenNear, screenDepth);
    //eyeData.mProj = Matrix::CreatePerspectiveFieldOfView(((float)Pi / 4.0f), ((float)texWidth / (float)texHeight), screenNear, screenDepth);
    
    LightComponent::Initialize();
}

void DirectionalLightComponent::DestroyResource()
{
    LightComponent::DestroyResource();
    if (lightBuffer) lightBuffer->Release();
}

void DirectionalLightComponent::UpdateSubresource(DirectionLightData Data)
{
    engInst->GetContext()->UpdateSubresource(lightBuffer, 0, nullptr, &Data, 0, 0);

    engInst->GetContext()->VSSetConstantBuffers(1, 1, &lightBuffer);
    engInst->GetContext()->PSSetConstantBuffers(1, 1, &lightBuffer);
}

void DirectionalLightComponent::UpdateShaderResources()
{
    engInst->GetContext()->PSSetShaderResources(1, 1, &m_pOutputTextureSRV);
    engInst->GetContext()->PSSetSamplers(1, 1, &SampShadow);
}

void DirectionalLightComponent::Update(float timeTick)
{
    LightComponent::Update(timeTick);
    //Vector3 rot(-90.0f, 0.0f, 0.0f);
    //lightData.direction = Vector4::Transform(Vector4(-3.0f, -2.0f, -0.5f, 0.0f), Quaternion::CreateFromYawPitchRoll(RadiansFromDegree(rot)));
    //UpdateSubresource(lightData);

    const auto forward = transform.GetForward();
    lightData.direction = Vector4(forward.x, forward.y, forward.z, 0.0f);
}

void DirectionalLightComponent::Draw()
{
    //LightComponent::Draw();
}
