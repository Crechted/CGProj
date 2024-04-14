#include "TextureComponent.h"

#include "WICTextureLoader.h"
#include "Core/Engine.h"

TextureComponent::TextureComponent(const wchar_t* path) : pathForLoad(path)
{
}

void TextureComponent::Initialize()
{
    GameComponent::Initialize();
    CreateSampler();
}

void TextureComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (textureRV) textureRV->Release();
    if (samplerLinear) samplerLinear->Release();
}

void TextureComponent::PreDraw()
{
    GameComponent::PreDraw();
    engInst->GetContext()->PSSetShaderResources(0, 1, &textureRV);
    engInst->GetContext()->PSSetSamplers(0, 1, &samplerLinear);
}

void TextureComponent::CreateSampler()
{
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.BorderColor[0] = 1.0f;
    sampDesc.BorderColor[1] = 0.0f;
    sampDesc.BorderColor[2] = 0.0f;
    sampDesc.BorderColor[3] = 1.0f;

    engInst->GetDevice()->CreateSamplerState(&sampDesc, &samplerLinear);

    LoadTexture(pathForLoad);
    if (!textureRV) LoadDefaultTexture();
}

void TextureComponent::Draw()
{
    GameComponent::Draw();
}

void TextureComponent::SetTexture(ID3D11ShaderResourceView* tex)
{
    if (tex)
    {
        if (textureRV) textureRV->Release();
        textureRV = tex;
    }
}

void TextureComponent::SetTexture(const wchar_t* path)
{
    pathForLoad = path;
}

void TextureComponent::LoadDefaultTexture()
{
    LoadTexture(L"Resource/Textures/DefaultCG.dds");
}

void TextureComponent::LoadTexture(const wchar_t* path)
{
    if (textureRV) textureRV->Release();
    auto res = DirectX::CreateDDSTextureFromFile(engInst->GetDevice(), path, nullptr, &textureRV);

    if (FAILED(res))
    {
        res = DirectX::CreateWICTextureFromFile(engInst->GetDevice(), path, nullptr, &textureRV);
        if (FAILED(res))
            printf("Load texture failed");
    }

}
