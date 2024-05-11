#include "TextureComponent.h"

#include "WICTextureLoader.h"
#include "Core/Engine.h"

static Array<LoadedTex> LoadedTextures;

TextureComponent::TextureComponent(const wchar_t* path)
    : pathForLoad(path)
{
}

void TextureComponent::Initialize()
{
    GameComponent::Initialize();
    D3D11_BUFFER_DESC constBufDesc;
    constBufDesc.Usage = D3D11_USAGE_DEFAULT;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = 0;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(Material);
    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &materialBuffer);

    CreateSampler();
}

void TextureComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (samplerLinear) samplerLinear->Release();
    if (materialBuffer) materialBuffer->Release();
}

void TextureComponent::PreDraw()
{
    GameComponent::PreDraw();
    engInst->GetContext()->PSSetShaderResources(2, 1, &textureRV);
    engInst->GetContext()->PSSetSamplers(0, 1, &samplerLinear);    
    engInst->GetContext()->UpdateSubresource(materialBuffer, 0, nullptr, &material, 0, 0);
    engInst->GetContext()->PSSetConstantBuffers(2, 1, &materialBuffer);
}

void TextureComponent::CreateSampler()
{
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

Array<LoadedTex> TextureComponent::GetLoadTextures() { return LoadedTextures; }

ID3D11ShaderResourceView* TextureComponent::FindTexture(const wchar_t* name)
{
    for (const auto& tex : LoadedTextures)
    {
        if (tex.name != name) continue;
        return tex.textureRV;
    }
    return nullptr;
}

void TextureComponent::AddTexture(const LoadedTex& texData)
{
    if (!ContainsTextures(texData.name)) LoadedTextures.insert(texData);
}

void TextureComponent::AddTexture(const wchar_t* name, ID3D11ShaderResourceView* resource)
{
    if (!ContainsTextures(name)) LoadedTextures.insert({name, resource});
}

void TextureComponent::RemoveTexture(const LoadedTex& texData)
{
    if (ContainsTextures(texData.name))
    {
        LoadedTextures.remove(texData);
        if (texData.textureRV) texData.textureRV->Release();
    }
}

void TextureComponent::RemoveTexture(const wchar_t* name, ID3D11ShaderResourceView* resource)
{
    if (ContainsTextures(name))
    {
        LoadedTextures.remove({name, resource});
        if (resource) resource->Release();
    }
}

bool TextureComponent::ContainsTextures(const wchar_t* name)
{
    for (const auto& tex : LoadedTextures)
    {
        if (tex.name != name) continue;
        return true;
    }
    return false;
}

bool TextureComponent::ContainsTextures(ID3D11ShaderResourceView* resource)
{
    for (const auto& tex : LoadedTextures)
    {
        if (tex.textureRV != resource) continue;
        return true;
    }
    return false;
}

void TextureComponent::Clear()
{
    for (const auto& tex : LoadedTextures)
    {
        if (tex.textureRV) tex.textureRV->Release();
    }
}

void TextureComponent::LoadDefaultTexture()
{
    LoadTexture(L"Resource/Textures/DefaultCG.dds");
}

void TextureComponent::LoadTexture(const wchar_t* path)
{
    if (textureRV)
    {
        textureRV->Release();
        textureRV = nullptr;
    }
    textureRV = FindTexture(path);
    if (textureRV) return;

    auto res = DirectX::CreateDDSTextureFromFile(engInst->GetDevice(), path, nullptr, &textureRV);

    if (FAILED(res))
    {
        res = DirectX::CreateWICTextureFromFile(engInst->GetDevice(), path, nullptr, &textureRV);
        if (FAILED(res)) printf("Load texture failed");
        else LoadedTextures.insert(LoadedTex{path, textureRV});
    }
    else LoadedTextures.insert(LoadedTex{path, textureRV});
}
