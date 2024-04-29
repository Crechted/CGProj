#include "RenderTarget.h"

#include "Core/Engine.h"

RenderTarget::RenderTarget(TargetViewType targetType, int32_t textureWidth, int32_t textureHeight)
    : targetType(targetType), width(textureWidth), height(textureHeight)
{
    engInst = &Engine::GetInstance();
}

RenderTarget::~RenderTarget()
{
    Destroy();
}

ID3D11ShaderResourceView* RenderTarget::GetShaderResourceView()
{
    if (targetType == TargetViewType::Texture) return shaderResourceView;
    return nullptr;
}

void RenderTarget::Destroy()
{
    if (renderTargetTexture) renderTargetTexture->Release();
    if (renderTargetView) renderTargetView->Release();
    if (shaderResourceView) shaderResourceView->Release();
}

void RenderTarget::Initialize()
{
    if (targetType == TargetViewType::Texture) CreateRenderTargetToTexture(width, height);
    if (targetType == TargetViewType::BackBuffer)
    {
        ID3D11Texture2D* backTex;
        engInst->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex); // __uuidof(ID3D11Texture2D)
        engInst->GetDevice()->CreateRenderTargetView(backTex, nullptr, &renderTargetView);
    }
    outputViewPort.TopLeftX = 0.0f;
    outputViewPort.TopLeftY = 0.0f;
    outputViewPort.Width = static_cast<float>(width);
    outputViewPort.Height = static_cast<float>(height);
    outputViewPort.MinDepth = 0.0f;
    outputViewPort.MaxDepth = 1.0f;
}

void RenderTarget::SetTarget()
{
    engInst->GetContext()->OMSetRenderTargets(1, &renderTargetView, nullptr);
    engInst->GetContext()->RSSetViewports(1, &outputViewPort);
}

void RenderTarget::ClearTarget()
{
    float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    engInst->GetContext()->ClearRenderTargetView(renderTargetView, color);
}

bool RenderTarget::CreateRenderTargetToTexture(int32_t textureWidth, int32_t textureHeight)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // описание render target (цели рендера)
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Создание текстуры
    HRESULT result = engInst->GetDevice()->CreateTexture2D(&textureDesc, NULL, &renderTargetTexture);
    if (FAILED(result))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    // Описание render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Создание render target view.
    result = engInst->GetDevice()->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
    if (FAILED(result))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    // Описание shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Создание shader resource view.
    result = engInst->GetDevice()->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &shaderResourceView);
    if (FAILED(result))
        return false;

    return true;
}
