#include "LightComponent.h"

#include <string>

#include "Core/Engine.h"
#include "Core/Components/TextureComponent.h"

void LightComponent::Initialize()
{
    HRESULT hr;

    // ******************
    // Create a depth/stencil buffer or shadow map with the same width and height as the texture, and the corresponding view
    //
    /*D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Format = m_ShadowMap ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.Width = texWidth;
    texDesc.Height = texHeight;
    texDesc.ArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | (m_ShadowMap ? D3D11_BIND_SHADER_RESOURCE : 0);*/
    CD3D11_TEXTURE2D_DESC texDesc((m_ShadowMap ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_D24_UNORM_S8_UINT),
            texWidth, texHeight, 1, 1,
            D3D11_BIND_DEPTH_STENCIL | (m_ShadowMap ? D3D11_BIND_SHADER_RESOURCE : 0));
    
    hr = engInst->GetDevice()->CreateTexture2D(&texDesc, nullptr, &depthTex);
    if (FAILED(hr)) return;

    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(depthTex
        , D3D11_DSV_DIMENSION_TEXTURE2D
        , DXGI_FORMAT_D24_UNORM_S8_UINT);

    hr = engInst->GetDevice()->CreateDepthStencilView(depthTex, &dsvDesc,
        &m_pOutputTextureDSV);
    if (FAILED(hr)) return;

    if (m_ShadowMap)
    {
        // SRV of shadow map
        CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(depthTex
            , D3D11_SRV_DIMENSION_TEXTURE2D
            , DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

        hr = engInst->GetDevice()->CreateShaderResourceView(depthTex, &srvDesc,
            &m_pOutputTextureSRV);
        if (FAILED(hr)) return;
    }

    // Sampler status: depth comparison and Border mode
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampDesc.BorderColor[0] = {1.0f};
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    engInst->GetDevice()->CreateSamplerState(&sampDesc, &SampShadow);

    // ******************
    // Initialize the viewport
    //
    m_OutputViewPort.TopLeftX = 0.0f;
    m_OutputViewPort.TopLeftY = 0.0f;
    m_OutputViewPort.Width = static_cast<float>(texWidth);
    m_OutputViewPort.Height = static_cast<float>(texHeight);
    m_OutputViewPort.MinDepth = 0.0f;
    m_OutputViewPort.MaxDepth = 1.0f;

    GameComponent::Initialize();
}

void LightComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (m_pOutputTextureSRV) m_pOutputTextureSRV->Release();
    if (m_pOutputTextureRTV) m_pOutputTextureRTV->Release();
    if (m_pOutputTextureDSV) m_pOutputTextureDSV->Release();
    if (depthTex) depthTex->Release();
    if (SampShadow) SampShadow->Release();
    if (m_pCacheRTV) m_pCacheRTV->Release();
    if (m_pCacheDSV) m_pCacheDSV->Release();
}

void LightComponent::Draw()
{
    GameComponent::Draw();
}

void LightComponent::AddShadowMap()
{
    TextureComponent::AddTexture(shadowMapName, GetOutputTexture());
    
}

void LightComponent::RemoveShadowMap()
{
    //TextureComponent::RemoveTexture(shadowMapName, GetOutputTexture());
}

void LightComponent::Begin()
{

    RemoveShadowMap();
    // Cache rendering target and depth template view
    engInst->GetContext()->OMGetRenderTargets(1, &m_pCacheRTV, &m_pCacheDSV);
    // cache viewport
    UINT num_Viewports = 1;
    engInst->GetContext()->RSGetViewports(&num_Viewports, &m_CacheViewPort);

    // Clear the buffer
    // ... 
    engInst->GetContext()->ClearDepthStencilView(m_pOutputTextureDSV, D3D11_CLEAR_DEPTH | (m_ShadowMap ? 0 : D3D11_CLEAR_STENCIL), 1.0f, 0);

    // Set rendering target and depth template view
    engInst->GetContext()->OMSetRenderTargets((m_ShadowMap ? 0 : 1),
        (m_ShadowMap ? nullptr : &m_pOutputTextureRTV),
        m_pOutputTextureDSV);
    // Set the viewport
    engInst->GetContext()->RSSetViewports(num_Viewports, &m_OutputViewPort);
}

void LightComponent::End()
{
    AddShadowMap();
    float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    //engInst->GetContext()->ClearRenderTargetView(m_pOutputTextureRTV, color);
    //engInst->GetContext()->ClearDepthStencilView(m_pCacheDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    /*CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = cullMode; // D3D11_CULL_NONE
    rastDesc.FillMode = fillMode; // D3D11_FILL_SOLID
    rastDesc.AntialiasedLineEnable = isAntialiasedLine;
    /*rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.DepthBias = 100000;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.SlopeScaledDepthBias = 1.0f;#1#
    auto res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastState);
    engInst->GetContext()->RSSetState(rastState);*/
}
