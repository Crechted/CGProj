#pragma once
#include <cstdint>
#include <d3d11.h>

class Engine;

enum class TargetViewType
{
    BackBuffer,
    Texture
};

class RenderTarget
{
public:
    RenderTarget(TargetViewType targetType = TargetViewType::Texture, int32_t textureWidth = 800, int32_t textureHeight = 800);
    virtual ~RenderTarget();

    void SetTargetType(TargetViewType type) { if (!renderTargetView) targetType = type; }
    TargetViewType GetTargetType() const { return targetType; }
    ID3D11RenderTargetView* GetRenderTargetView() const {return renderTargetView;}

    ID3D11ShaderResourceView* GetShaderResourceView();
    void Initialize();
    void SetTarget();
    void ClearTarget();
    void Destroy();
    int32_t GetHeight() const { return height; }
    int32_t GetWidth() const { return width; }
    void SetWidth(int32_t newWidth) { width = newWidth ? newWidth : width; }
    void SetHeight(int32_t newHeight) { height = newHeight ? newHeight : height; }

protected:
    Engine* engInst;
    TargetViewType targetType;
    int32_t width = 800;
    int32_t height = 800;
    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11ShaderResourceView* shaderResourceView = nullptr;
    D3D11_VIEWPORT outputViewPort = {};
    bool CreateRenderTargetToTexture(int32_t textureWidth, int32_t textureHeight);
};
