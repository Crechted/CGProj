#pragma once

#include "DDSTextureLoader.h"
#include "GameComponent.h"

struct LoadedTex
{
    const wchar_t* path;
    ID3D11ShaderResourceView* textureRV;
};

static Array<LoadedTex> LoadedTextures;

class TextureComponent : public GameComponent
{
public:
    TextureComponent(const wchar_t* path = nullptr);
    void CreateSampler();
    void Initialize() override;
    void DestroyResource() override;
    void PreDraw() override;
    void Draw() override;

    void SetTexture(ID3D11ShaderResourceView* tex);
    void SetTexture(const wchar_t* path);
    
protected:
    void LoadTexture(const wchar_t* path);
    D3D11_SAMPLER_DESC sampDesc;

private:
    ID3D11ShaderResourceView* textureRV = nullptr;
    ID3D11SamplerState* samplerLinear = nullptr;

    const wchar_t* defPath;
    const wchar_t* pathForLoad;
    void LoadDefaultTexture();
};
