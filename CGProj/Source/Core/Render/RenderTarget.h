#pragma once
#include <cstdint>
#include <d3d11.h>

#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

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
    ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView; }
    ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView; }

    ID3D11ShaderResourceView* GetRenderTargetSRV() const;
    ID3D11ShaderResourceView* GetDepthStencilSRV() const;

    ID3D11Texture2D* GetRenderTargetTexture() const;
    ID3D11Texture2D* GetDepthStencilTexture() const;

    void CopyDepthStencilView(ID3D11DepthStencilView* DSV, ID3D11ShaderResourceView* depthSRV, ID3D11Texture2D* depthTex );
    
    void CreateAll();

    void CreateRenderTarget(DXGI_FORMAT texFormat = DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT rtvFormat = DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT renderSRVFormat = DXGI_FORMAT_R32G32B32A32_FLOAT);

    void CreateDepthStencilView(DXGI_FORMAT texFormat = DXGI_FORMAT_R24G8_TYPELESS,
        DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
        DXGI_FORMAT depthSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

    void CreateDepthStencilState(BOOL depthEnable = true,
        D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS,
        BOOL stencilEnable = false,
        UINT8 stencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
        UINT8 stencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
        D3D11_STENCIL_OP frontStencilFailOp = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP frontStencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP frontStencilPassOp = D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_FUNC frontStencilFunc = D3D11_COMPARISON_ALWAYS,
        D3D11_STENCIL_OP backStencilFailOp = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP backStencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP backStencilPassOp = D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_FUNC backStencilFunc = D3D11_COMPARISON_ALWAYS);
    
    void BindTarget(bool bindStencilView = true);
    void BindDepthStencil();
    void SetDepthStencilState(ID3D11DepthStencilState* state = nullptr, uint32_t stencilRef = 0);
    void Clear();
    void ClearTarget(Vector4 color = Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    void ClearDepthStencil(UINT ClearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
    void Destroy();
    int32_t GetHeight() const { return height; }
    int32_t GetWidth() const { return width; }
    void SetWidth(int32_t newWidth) { width = newWidth ? newWidth : width; }
    void SetHeight(int32_t newHeight) { height = newHeight ? newHeight : height; }
    D3D11_VIEWPORT GetViewport() const { return outputViewPort; }

protected:
    Engine* engInst;
    TargetViewType targetType;
    int32_t width;
    int32_t height;
    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11ShaderResourceView* renderSRV = nullptr;

    ID3D11Texture2D* depthStencilTex = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;
    ID3D11ShaderResourceView* depthStencilSRV = nullptr;

    D3D11_VIEWPORT outputViewPort = {};
    bool CreateRenderTargetToTexture(DXGI_FORMAT texFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT renderSRVFormat);
};
