#include "PostRenderGameMap.h"

#include "Core/Engine.h"
#include "Core/Render/RenderTarget.h"

PostRenderGameMap::PostRenderGameMap(const LPCWSTR shaderPath, ID3D11ShaderResourceView* imageSRV, Vector2 viewportDim, Vector2 viewportPos,
    Vector2 viewportDepth)
    : PostRenderImage(imageSRV, viewportDim, viewportPos, viewportDepth)
{
    this->shaderPath = shaderPath;
    mapCamera = engInst->CreateCamera(ViewType::OrtXOZ);
    //mapCamera->viewType = ViewType::OrtXOZ;
    mapCamera->heightOrt = 20.0f;
    mapCamera->widthOrt = 20.0f;
}

void PostRenderGameMap::Destroy()
{
    if (mapRenderTarget) mapRenderTarget->Destroy();
    PostRenderImage::Destroy();
}

void PostRenderGameMap::Update(float timeTick)
{
    mapCamera->Update(timeTick);
    PostRenderImage::Update(timeTick);
}

void PostRenderGameMap::Initialize()
{
    SetShaderPath(shaderPath);
    mapRenderTarget = new RenderTarget(TargetViewType::Texture, static_cast<int32_t>(viewport.Width),
        static_cast<int32_t>(viewport.Height));
    CD3D11_DEPTH_STENCIL_DESC descDSS(D3D11_DEFAULT);
    descDSS.DepthEnable = true;
    descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    descDSS.StencilEnable = true;
    descDSS.StencilReadMask = 1;
    descDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    descDSS.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    mapRenderTarget->CreateAll();
    mapRenderTarget->CreateDepthStencilState(&descDSS);

    mapCamera->GetSceneComponent()->SetLocation({0.0f, -3.0f, 0.0f});
    mapCamera->GetSceneComponent()->attachRotate = false;
    mapCamera->GetSceneComponent()->AttachTo(engInst->GetCurCamera()->GetSceneComponent()->GetParentComponent()->GetParentComponent());
    PostRenderImage::Initialize();
}

void PostRenderGameMap::Draw()
{
    const auto PrevCam = engInst->GetCurCamera();
    const auto eye = mapCamera->GetEyeData();
    engInst->SetCurCamera(mapCamera);
    engInst->SetCurEyeData(eye);
    mapRenderTarget->BindTarget();
    mapRenderTarget->BindDepthStencilState();
    mapRenderTarget->ClearTarget();
    //engInst->SetRenderState(RenderState::DrawDebug);
    engInst->RenderScene(true, true);
    engInst->GetBackBufferRTV()->BindTarget();
    SetSRV(mapRenderTarget->GetRenderTargetSRV());
    //SetSRV(engInst->GetTexRenderTarget()->GetRenderTargetSRV());
    engInst->SetCurCamera(PrevCam);
    PostRenderImage::Draw();
}

ID3D11ShaderResourceView* PostRenderGameMap::GetMapSRV() const
{
    return mapRenderTarget->GetRenderTargetSRV();
}
