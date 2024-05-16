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

ID3D11ShaderResourceView* RenderTarget::GetRenderTargetSRV() const
{
    if (targetType == TargetViewType::Texture) return renderSRV;
    return nullptr;
}

ID3D11ShaderResourceView* RenderTarget::GetDepthStencilSRV() const
{
    return depthStencilSRV;
}

ID3D11Texture2D* RenderTarget::GetRenderTargetTexture() const
{
    return renderTargetTexture;
}

ID3D11Texture2D* RenderTarget::GetDepthStencilTexture() const
{
    return depthStencilTex;
}

void RenderTarget::CopyDepthStencilView(ID3D11DepthStencilView* DSV, ID3D11ShaderResourceView* depthSRV, ID3D11Texture2D* depthTex)
{
    /*if (!DSV || !depthSRV || !depthTex) return;
    if (depthStencilView) depthStencilView->Release();
    if (depthStencilSRV) depthStencilSRV->Release();
    if (depthStencilTex) depthStencilTex->Release();
    depthStencilView = DSV;
    depthStencilSRV = depthSRV;
    depthStencilTex = depthTex;*/
    engInst->GetContext()->CopyResource(depthStencilTex, depthTex);
}

void RenderTarget::CreateAll()
{
    CreateRenderTarget();
    CreateDepthStencilView();
    CreateDepthStencilState();
}

void RenderTarget::CreateRenderTarget(DXGI_FORMAT texFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT renderSRVFormat)
{
    if (renderTargetView) return;

    if (targetType == TargetViewType::Texture) CreateRenderTargetToTexture(texFormat, rtvFormat, renderSRVFormat);
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

void RenderTarget::CreateDepthStencilView(DXGI_FORMAT texFormat, DXGI_FORMAT dsvFormat, DXGI_FORMAT depthSRVFormat)
{
    if (depthStencilView) return;

    HRESULT res;
    D3D11_TEXTURE2D_DESC textureDesc = {};
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = texFormat; // DXGI_FORMAT_D24_UNORM_S8_UINT
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    res = engInst->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &depthStencilTex);
    if (FAILED(res)) return;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = dsvFormat;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    res = engInst->GetDevice()->CreateDepthStencilView(depthStencilTex, &descDSV, &depthStencilView);
    if (FAILED(res)) return;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = depthSRVFormat;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    res = engInst->GetDevice()->CreateShaderResourceView(depthStencilTex, &shaderResourceViewDesc, &depthStencilSRV);
    if (FAILED(res)) return;

}

void RenderTarget::CreateDepthStencilState(BOOL depthEnable,
    D3D11_DEPTH_WRITE_MASK depthWriteMask,
    D3D11_COMPARISON_FUNC depthFunc,
    BOOL stencilEnable,
    UINT8 stencilReadMask,
    UINT8 stencilWriteMask,
    D3D11_STENCIL_OP frontStencilFailOp,
    D3D11_STENCIL_OP frontStencilDepthFailOp,
    D3D11_STENCIL_OP frontStencilPassOp,
    D3D11_COMPARISON_FUNC frontStencilFunc,
    D3D11_STENCIL_OP backStencilFailOp,
    D3D11_STENCIL_OP backStencilDepthFailOp,
    D3D11_STENCIL_OP backStencilPassOp,
    D3D11_COMPARISON_FUNC backStencilFunc)
{
    CD3D11_DEPTH_STENCIL_DESC descDS(depthEnable, depthWriteMask, depthFunc, stencilEnable, stencilReadMask, stencilWriteMask,
        frontStencilFailOp, frontStencilDepthFailOp, frontStencilPassOp, frontStencilFunc,
        backStencilFailOp, backStencilDepthFailOp, backStencilPassOp, backStencilFunc);
    descDS.DepthEnable = TRUE;
    descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    descDS.DepthFunc = D3D11_COMPARISON_LESS;
    descDS.StencilEnable = FALSE;
    descDS.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    descDS.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    const D3D11_DEPTH_STENCILOP_DESC defaultStencilOp =
        {D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS};
    descDS.FrontFace = defaultStencilOp;
    descDS.BackFace = defaultStencilOp;
    engInst->GetDevice()->CreateDepthStencilState(&descDS, &depthStencilState);
}

void RenderTarget::Destroy()
{
    if (renderTargetTexture) renderTargetTexture->Release();
    if (renderTargetView) renderTargetView->Release();
    if (renderSRV) renderSRV->Release();
    if (depthStencilTex) depthStencilTex->Release();
    if (depthStencilView) depthStencilView->Release();
    if (depthStencilSRV) depthStencilSRV->Release();
}

void RenderTarget::BindTarget(bool bindStencilView)
{
    engInst->GetContext()->OMSetRenderTargets(1, &renderTargetView, bindStencilView ? depthStencilView : nullptr);
    engInst->GetContext()->RSSetViewports(1, &outputViewPort);
}

void RenderTarget::BindDepthStencil()
{
    engInst->GetContext()->OMSetRenderTargets(0, nullptr, depthStencilView);
    engInst->GetContext()->RSSetViewports(1, &outputViewPort);
}

void RenderTarget::SetDepthStencilState(ID3D11DepthStencilState* state, uint32_t stencilRef)
{
    engInst->GetContext()->OMSetDepthStencilState(state ? state : depthStencilState, stencilRef);
}

void RenderTarget::Clear()
{
    ClearTarget();
    if (depthStencilView) ClearDepthStencil();
}

void RenderTarget::ClearTarget(Vector4 color)
{
    float col[4] = {color.x, color.y, color.z, color.w};
    engInst->GetContext()->ClearRenderTargetView(renderTargetView, col);
}

void RenderTarget::ClearDepthStencil(UINT ClearFlags)
{
    engInst->GetContext()->ClearDepthStencilView(depthStencilView, ClearFlags, 1.0f, 0);
}

bool RenderTarget::CreateRenderTargetToTexture(DXGI_FORMAT texFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT renderSRVFormat)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = texFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT result = engInst->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &renderTargetTexture);
    if (FAILED(result))
        return false;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = rtvFormat;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    result = engInst->GetDevice()->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
    if (FAILED(result))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = renderSRVFormat;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = engInst->GetDevice()->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &renderSRV);
    if (FAILED(result))
        return false;

    return true;
}
