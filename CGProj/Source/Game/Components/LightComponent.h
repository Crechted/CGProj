#pragma once

#include <d3d11.h>
#include "Core/Components/GameComponent.h"
#include "Game/Camera.h"

class LightComponent : public GameComponent
{
public:
    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;

    ID3D11ShaderResourceView* GetOutputTexture() const {return m_pOutputTextureSRV;}

    virtual void Begin();
    virtual void End();

    EyeViewData GetEyeData() const {return eyeData;}

protected:
    ID3D11ShaderResourceView* m_pOutputTextureSRV = nullptr; // Shader resource view corresponding to the output texture (or shadow map)
    ID3D11RenderTargetView* m_pOutputTextureRTV = nullptr;   // Render target view corresponding to the output texture
    ID3D11DepthStencilView* m_pOutputTextureDSV = nullptr;   // Depth/template view (or shadow map) used for output texture
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_VIEWPORT m_OutputViewPort = {};                    // Viewport used for output

    D3D11_SAMPLER_DESC sampDesc;
    ID3D11SamplerState* SampShadow = nullptr;
    
    ID3D11RenderTargetView* m_pCacheRTV = nullptr; // Temporary cache back buffer
    ID3D11DepthStencilView* m_pCacheDSV = nullptr; // Temporary buffer depth/template buffer
    D3D11_VIEWPORT m_CacheViewPort = {};           // Temporarily cached viewport

    EyeViewData eyeData;
    
    bool m_GenerateMips = false; // Whether to generate mipmap chain
    bool m_ShadowMap = true;
    int32_t texWidth = 2048;
    int32_t texHeight = 2048;

    const wchar_t* shadowMapName = L"T_ShadowMap";
    
private:
    void AddShadowMap();
    void RemoveShadowMap();
};



