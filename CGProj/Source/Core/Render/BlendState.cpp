#include "BlendState.h"

#include "Core/Engine.h"

BlendState::BlendState()
{
    engInst = &Engine::GetInstance();
}

BlendState::~BlendState()
{
    Destroy();
}

void BlendState::Destroy()
{
    if (blendState) blendState->Release();
}

BlendState* BlendState::CreateBlendState()
{
    const CD3D11_BLEND_DESC blendStateDesc(D3D11_DEFAULT);
    return CreateBlendState(&blendStateDesc);
}

BlendState* BlendState::CreateBlendState(const D3D11_BLEND_DESC* blendStateDesc)
{
    engInst->GetDevice()->CreateBlendState(blendStateDesc, &blendState);
    return this;
}

BlendState* BlendState::CreateAdditive()
{
    D3D11_BLEND_DESC blendStateDesc = {};
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    return CreateBlendState(&blendStateDesc);
}

BlendState* BlendState::CreateAlphaBlend()
{
    D3D11_BLEND_DESC blendStateDesc = {};
    ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    return CreateBlendState(&blendStateDesc);
}

void BlendState::Bind(const FLOAT blendFactor[4], UINT sampleMask)
{
    const float bf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const float* res = blendFactor ? blendFactor : bf;
    engInst->GetContext()->OMSetBlendState(blendState, res, sampleMask);
}

void BlendState::UnBind(const FLOAT blendFactor[4], UINT sampleMask)
{
    const float bf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    const float* res = blendFactor ? blendFactor : bf;
    engInst->GetContext()->OMSetBlendState(nullptr, res, sampleMask);
}
