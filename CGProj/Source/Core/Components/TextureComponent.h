#pragma once

#include "DDSTextureLoader.h"
#include "GameComponent.h"

struct LoadedTex
{
    bool operator==(const LoadedTex& other) const
    {
        return textureRV == other.textureRV && other.name == name;
    }
    const wchar_t* name;
    ID3D11ShaderResourceView* textureRV;
};


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

    static Array<LoadedTex> GetLoadTextures();
    static ID3D11ShaderResourceView* FindTexture(const wchar_t* name);
    static void AddTexture(const LoadedTex& texData);
    static void AddTexture(const wchar_t* name, ID3D11ShaderResourceView* resource);
    static void RemoveTexture(const LoadedTex& texData);
    static void RemoveTexture(const wchar_t* name, ID3D11ShaderResourceView* resource);
    static bool ContainsTextures(const wchar_t* name);
    static bool ContainsTextures(ID3D11ShaderResourceView* resource);
    static void Clear();
    
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
