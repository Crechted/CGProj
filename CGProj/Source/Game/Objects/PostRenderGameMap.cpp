#include "PostRenderGameMap.h"

#include "Core/Engine.h"
#include "Core/Render/RenderTarget.h"

PostRenderGameMap::PostRenderGameMap(const LPCWSTR shaderPath, ID3D11ShaderResourceView* imageSRV, Vector2 viewportDim, Vector2 viewportPos, Vector2 viewportDepth)
: PostRenderImage(imageSRV, viewportDim, viewportPos, viewportDepth)
{
    this->shaderPath = shaderPath;
    mapCamera = engInst->CreateCamera(ViewType::OrtXOZ);
    //mapCamera->viewType = ViewType::OrtXOZ;
    mapCamera->heightOrt = 20.0f;
    mapCamera->widthOrt = 20.0f;
}

void PostRenderGameMap::Update(float timeTick)
{
    mapCamera->Update(timeTick);
    PostRenderImage::Update(timeTick);
}

void PostRenderGameMap::Initialize()
{
    SetShaderPath(shaderPath);
    SetVisibility(true);    
    mapRenderTarget = new RenderTarget(TargetViewType::Texture, static_cast<int32_t>(viewport.Width), static_cast<int32_t>(viewport.Height));
    mapRenderTarget->CreateAll();
    mapCamera->Initialize();
    mapCamera->GetSceneComponent()->SetLocation({0.0f, -3.0f, 0.0f});
    mapCamera->GetSceneComponent()->attachRotate = false;
    mapCamera->GetSceneComponent()->AttachTo(engInst->GetCurCamera()->GetSceneComponent()->GetParentComponent()->GetParentComponent());
    PostRenderImage::Initialize();
}

void PostRenderGameMap::Draw()
{
    const auto PrevCam = engInst->GetCurCamera();
    engInst->SetCurCamera(mapCamera);
    const auto eye = mapCamera->GetEyeData();
    engInst->SetCurEyeData(eye);
    mapRenderTarget->BindTarget(true);
    mapRenderTarget->ClearTarget();
    engInst->RenderScene();
    engInst->GetBackBufferRTV()->BindTarget();
    SetSRV(mapRenderTarget->GetRenderTargetSRV());
    //SetSRV(engInst->GetTexRenderTarget()->GetRenderTargetSRV());
    engInst->SetCurCamera(PrevCam);
    PostRenderImage::Draw();
}
