#include "LightComponent.h"

#include "Core/Engine.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Components/TextureComponent.h"

LightComponent::LightComponent()
{
    sceneComponent = CreateComponent<SceneComponent>();
}

void LightComponent::Initialize()
{
    CreateShadowMappingData();
    CreateLightBuffer();
    if (!volumeVertices.isEmpty()) CreateVertices();
    GameComponent::Initialize();

    const auto location = sceneComponent->GetWorldLocation();
    const auto forward = sceneComponent->GetWorldTransform().GetForward();
    const auto camData = engInst->GetCurCamera()->GetEyeData();
    const auto camViewProj = camData.GetViewProj();

    lightData.mViewProj = eyeData.GetViewProj();
    lightData.posWS = Vector4(location.x, location.y, location.z, 1.0f);
    lightData.posVS = Vector4::Transform(lightData.posWS, camViewProj);
    lightData.directionWS = Vector4(forward.x, forward.y, forward.z, 0.0f);
    lightData.directionVS = Vector4::Transform(lightData.directionWS, camViewProj);
    lightData.color = Vector4::One;
}

void LightComponent::CreateShadowMappingData()
{
    HRESULT hr;

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_R24G8_TYPELESS
    texDesc.Width = texWidth;
    texDesc.Height = texHeight;
    texDesc.ArraySize = engInst->useCascadeShadow ? CASCADE_COUNT : 1;
    texDesc.MipLevels = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    texDesc.SampleDesc.Count = 1;

    hr = engInst->GetDevice()->CreateTexture2D(&texDesc, nullptr, &depthTex);
    if (FAILED(hr)) return;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT
    dsvDesc.ViewDimension = engInst->useCascadeShadow ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
    if (engInst->useCascadeShadow) dsvDesc.Texture2DArray = {0, 0, CASCADE_COUNT};

    hr = engInst->GetDevice()->CreateDepthStencilView(depthTex, &dsvDesc, &outputTextureDSV);
    if (FAILED(hr)) return;
    // SRV of shadow map
    /*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    srvDesc.ViewDimension = engInst->useCascadeShadow ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D ;
    if (engInst->useCascadeShadow) srvDesc.Texture2DArray = {0, 1, 0, 4};*/

    CD3D11_SHADER_RESOURCE_VIEW_DESC srvCDesc(depthTex,
        engInst->useCascadeShadow ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT);
    if (engInst->useCascadeShadow) srvCDesc.Texture2DArray = {0, 1, 0, CASCADE_COUNT};
    hr = engInst->GetDevice()->CreateShaderResourceView(depthTex, &srvCDesc, &outputTextureSRV);
    if (FAILED(hr)) return;

    // Sampler status: depth comparison and Border mode
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.BorderColor[0] = 0;
    sampDesc.BorderColor[1] = 0;
    sampDesc.BorderColor[2] = 0;
    sampDesc.BorderColor[3] = 0;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    engInst->GetDevice()->CreateSamplerState(&sampDesc, &sampShadow);

    // ******************
    // Initialize the viewport
    //
    outputViewPort.TopLeftX = 0.0f;
    outputViewPort.TopLeftY = 0.0f;
    outputViewPort.Width = static_cast<float>(texWidth);
    outputViewPort.Height = static_cast<float>(texHeight);
    outputViewPort.MinDepth = 0.0f;
    outputViewPort.MaxDepth = 1.0f;
}

void LightComponent::CreateLightBuffer()
{
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(LightData);
    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &lightBuffer);
}

void LightComponent::CreateVertices()
{
    CreateVertexBuffer();
    CreateIndexBuffer();
}

void LightComponent::CreateVertexBuffer()
{
    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(VertexNoTex) * volumeVertices.size();

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &volumeVertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuf);
}

void LightComponent::CreateIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufDesc;
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * volumeIndexes.size();

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &volumeIndexes[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &indexBuf);
}

void LightComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (outputTextureSRV) outputTextureSRV->Release();
    if (outputTextureDSV) outputTextureDSV->Release();
    if (depthTex) depthTex->Release();
    if (sampShadow) sampShadow->Release();
    if (lightBuffer) lightBuffer->Release();
    if (vertexBuf) vertexBuf->Release();
    if (indexBuf) indexBuf->Release();
}

void LightComponent::AddShadowMap()
{
    TextureComponent::AddTexture(shadowMapName, GetOutputTexture());

}

void LightComponent::RemoveShadowMap()
{
    //TextureComponent::RemoveTexture(shadowMapName, GetOutputTexture());
}

void LightComponent::UpdateSubresource()
{
}

void LightComponent::UpdateShaderResources()
{
    engInst->GetContext()->PSSetShaderResources(9, 1, &outputTextureSRV);
    engInst->GetContext()->PSSetSamplers(1, 1, &sampShadow);
}

void LightComponent::SetDepthStencil()
{
    engInst->GetContext()->OMSetRenderTargets(0, nullptr, outputTextureDSV);
    engInst->GetContext()->RSSetViewports(1, &outputViewPort);
}

void LightComponent::ClearDepthStencil()
{
    engInst->GetContext()->ClearDepthStencilView(outputTextureDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void LightComponent::Update(float timeTick)
{
    GameComponent::Update(timeTick);
    const auto location = sceneComponent->GetWorldLocation();
    const auto forward = sceneComponent->GetWorldTransform().GetForward();
    const auto camData = engInst->GetCurCamera()->GetEyeData();
    const auto viewProj = camData.GetViewProj();
    lightData.posWS = Vector4(location.x, location.y, location.z, 1.0f);
    lightData.posVS = Vector4::Transform(lightData.posWS, viewProj);
    lightData.directionWS = Vector4(forward.x, forward.y, forward.z, 0.0f);
    lightData.directionVS = Vector4::Transform(lightData.directionWS, viewProj);
}

void LightComponent::GetLightVolume(Array<VertexNoTex>& vertices, Array<int32_t>& indexes)
{
    vertices = volumeVertices;
    indexes = volumeIndexes;
}

void LightComponent::GetLightVolumeBuffers(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer)
{
    *vertexBuffer = vertexBuf;
    *indexBuffer = indexBuf;
}


Array<Vector4> CascadeShaderManager::GetFrustumCorners(const Matrix& view, const Matrix& proj)
{
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();

    Array<Vector4> corners;

    for (int32_t x = 0; x < 2; ++x)
    {
        for (int32_t y = 0; y < 2; ++y)
        {
            for (int32_t z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4(
                    2.0f * static_cast<float>(x) - 1.0f,
                    2.0f * static_cast<float>(y) - 1.0f,
                    static_cast<float>(z),
                    1.0f), inv);
                corners.insert(pt / pt.w);
            }
        }
    }
    return corners;
}

Vector3 CascadeShaderManager::GetFrustumCenter(const Array<Vector4>& corners)
{
    Vector3 center = Vector3::Zero;

    for (const auto& corner : corners)
    {
        center += Vector3(corner.x, corner.y, corner.z);
    }
    center /= static_cast<float>(corners.size());

    return center;
}

Matrix CascadeShaderManager::GetOrthographicProjByCorners(const Array<Vector4>& corners, const Matrix& lightView)
{
    if (corners.size() != 8) return Matrix::Identity;

    Vector4 FrustumMin;
    Vector4 FrustumMax;
    FrustumMin.x = FLT_MAX;
    FrustumMax.x = std::numeric_limits<float>::lowest();
    FrustumMin.y = FLT_MAX;
    FrustumMax.y = std::numeric_limits<float>::lowest();
    FrustumMin.z = FLT_MAX;
    FrustumMax.z = std::numeric_limits<float>::lowest();

    for (const auto& corner : corners)
    {
        Vector4 vTempTranslateCornerPoints = Vector4::Transform(corner, lightView);

        Vector4::Min(FrustumMin, vTempTranslateCornerPoints, FrustumMin);
        Vector4::Max(FrustumMax, vTempTranslateCornerPoints, FrustumMax);
    }

    const float zMult = 10.0f;
    FrustumMin.z = FrustumMin.z < 0 ? FrustumMin.z * zMult : FrustumMin.z / zMult;
    FrustumMax.z = FrustumMax.z < 0 ? FrustumMax.z / zMult : FrustumMax.z * zMult;

    const auto resOrthoMat = Matrix::CreateOrthographicOffCenter(FrustumMin.x, FrustumMax.x,
        FrustumMin.y, FrustumMax.y, FrustumMin.z, FrustumMax.z);

    return resOrthoMat;
}
