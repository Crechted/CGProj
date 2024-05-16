#pragma once

#include "DDSTextureLoader.h"
#include "GameComponent.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

struct LoadedTex
{
    bool operator==(const LoadedTex& other) const
    {
        return textureRV == other.textureRV && other.name == name;
    }

    const wchar_t* name;
    ID3D11ShaderResourceView* textureRV;
};

struct Material
{
    Vector4 globalAmbient = Vector4::One;
    Vector4 ambientColor = Vector4(0.01f);
    Vector4 emissiveColor;
    Vector4 diffuseColor;
    Vector4 specularColor;
    Vector4 reflectance;

    float opacity = 1.0f;
    float specularPower = 50.0f;
    float indexOfRefraction = 0.0f;
    alignas(4) bool hasAmbientTex = false;

    alignas(4) bool hasEmissiveTex = false;
    alignas(4) bool hasDiffuseTex = true;
    alignas(4) bool hasSpecularTex = false;
    alignas(4) bool hasSpecularPowerTex = false;

    alignas(4) bool hasNormalTex = false;
    alignas(4) bool hasBumpTex = false;
    alignas(4) bool hasOpacityTex = false;
    float bumpIntensity = 1.0f;

    float specularScale = 0.25f;
    float alphaThreshold = 0.01f;
    Vector2 padding;
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

    void SetMaterial(const Material& mat) { material = mat; }
    Material GetMaterial() const { return material; }
    void SetOpacity(float opacity) { material.opacity = opacity; }
    float GetOpacity() const { return material.opacity; }

protected:
    void LoadTexture(const wchar_t* path);
    D3D11_SAMPLER_DESC sampDesc;
    Material material;

private:
    ID3D11ShaderResourceView* textureRV = nullptr;
    ID3D11SamplerState* samplerLinear = nullptr;
    ID3D11Buffer* materialBuffer = nullptr;

    const wchar_t* defPath;
    const wchar_t* pathForLoad;
    void LoadDefaultTexture();
};
