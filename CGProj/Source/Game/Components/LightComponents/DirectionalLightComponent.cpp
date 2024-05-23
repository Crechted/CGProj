#include "DirectionalLightComponent.h"

#include "Game/Components/DrawBoxComponent.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"
#include "Core/Components/DrawComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Objects/Mesh.h"
#include "Game/Objects/PostRenderImage.h"
#include "Game/Objects/Drawable/DrawBox.h"


DirectionalLightComponent::DirectionalLightComponent()
{
    drawFrustum = CreateComponent<DrawBoxComponent>(Vector3::Zero, Vector3::Zero, Vector4::One, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    drawCascadeBox = CreateComponent<DrawBoxComponent>(Vector3::Zero, Vector3::Zero, Vector4(1.0f, 0.0f, 0.0f, 1.0f),
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    for (int32_t i = 0; i < CASCADE_COUNT; i++)
    {
        const auto size = 800.0f / CASCADE_COUNT;
        cascadeImgs.insert(engInst->CreateObject<PostRenderImage>(nullptr, Vector2(size, size), Vector2(size * i, 0.0f)));
    }
}

void DirectionalLightComponent::Initialize()
{
    if (engInst->useCascadeShadow)
        cascadeBuffer = (new Buffer())->CreateBuffer<CascadeData>();

    sceneComponent->initRotation = Vector3(-45.0f, 45.0f, 0.0f);

    Vector3 tar = sceneComponent->GetWorldMatrix().Translation() + sceneComponent->GetForward();
    eyeData.mView = Matrix::CreateLookAt(sceneComponent->GetLocation(), tar, sceneComponent->GetUp());

    const float screenNear = -50.0f;
    const float screenDepth = 50.0f;

    eyeData.mProj = Matrix::CreateOrthographic(50.0f, 50.0f, screenNear, screenDepth);
    eyesData[0].mView = eyeData.mView;
    eyesData[0].mProj = eyeData.mProj;
    lightData.type = 2;
    lightData.enabled = true;
    LightComponent::Initialize();
    CreateCascadeImage();
    engInst->GetInputDevice()->KeyDownDelegate.AddRaw(this, &DirectionalLightComponent::OnKeyDown);
}

void DirectionalLightComponent::CreateCascadeImage()
{
    for (int32_t i = 0; i < cascadeImgs.size(); ++i)
    {
        Buffer* addBufIndex = (new Buffer())->CreateBuffer<CascadeIndex>();
        const auto index = CascadeIndex{static_cast<uint32_t>(i)};
        addBufIndex->UpdateData(&index, 0);
        cascadeImgs[i]->SetSRV(outputTextureSRV);
        cascadeImgs[i]->SetUsageAdditionalBuffers(true);
        cascadeImgs[i]->SetVisibility(false);
        cascadeImgs[i]->SetShaderPath(L"./Resource/Shaders/CascadeImgShader.hlsl");
        cascadeImgs[i]->SetAdditionalBuffers(addBufIndex, 0, SPixel);
        addBufIndexes.insert(addBufIndex);
    }
}

void DirectionalLightComponent::OnKeyDown(Keys key)
{
    if ((engInst->GetInputDevice()->IsKeyDown(Keys::LeftShift) ||
         engInst->GetInputDevice()->IsKeyDown(Keys::RightShift)) && key == Keys::M)
    {
        for (const auto cascadeImg : cascadeImgs)
            cascadeImg->SetVisibility(!cascadeImg->IsVisible());
    }
}

void DirectionalLightComponent::DestroyResource()
{
    LightComponent::DestroyResource();
    for (const auto cascadeImg : cascadeImgs)
    {
        if (cascadeImg) cascadeImg->Destroy();
    }
    for (const auto addBufInd : addBufIndexes)
    {
        if (addBufInd) addBufInd->Destroy();
    }
}

void DirectionalLightComponent::UpdateLightData()
{
    LightComponent::UpdateLightData();

    if (engInst->useCascadeShadow)
    {
        cascadeBuffer->UpdateData(&cascData);
        cascadeBuffer->BindBuffers(3, SGeometry | SPixel);
    }
}

void DirectionalLightComponent::BindShadowMapSRV()
{
    engInst->GetContext()->PSSetShaderResources(engInst->useCascadeShadow ? 10 : 9, 1, &outputTextureSRV);
    engInst->GetContext()->PSSetSamplers(1, 1, &sampShadow);
}

void DirectionalLightComponent::Update(float timeTick)
{
    LightComponent::Update(timeTick);

    const auto forward = sceneComponent->GetWorldTransform().GetForward();

    if (engInst->useCascadeShadow)
    {
        const auto camData = engInst->GetCurCamera()->GetEyeData();
        float percentDist = 1.0f / static_cast<float>(CASCADE_COUNT);
        for (uint32_t i = 0; i < CASCADE_COUNT; ++i)
        {
            Matrix subProj;
            engInst->GetCurCamera()->GetProjAndDistanceBySection(percentDist * i, percentDist * static_cast<float>(i + 1), subProj,
                cascData.Distances[i]);
            //cascData.Distances[i] /= 50.0f;
            const auto corners = CascadeShaderManager::GetFrustumCorners(camData.mView, subProj);
            const auto center = CascadeShaderManager::GetFrustumCenter(corners);
            const auto tar = center + forward;
            const auto view = Matrix::CreateLookAt(center, tar, sceneComponent->GetUp());
            const auto proj = CascadeShaderManager::GetOrthographicProjByCorners(corners, view);
            eyesData[i].mView = view;
            eyesData[i].mProj = proj;
            cascData.ViewProj[i] = (eyesData[i].GetViewProj()).Transpose();
        }
    }
    else
    {
        const auto camData = engInst->GetCurCamera()->GetEyeData();
        Matrix subProj;
        engInst->GetCurCamera()->GetProjAndDistanceBySection(0.0f, 0.2f, subProj, cascData.Distances[0]);
        const auto corners = CascadeShaderManager::GetFrustumCorners(camData.mView, subProj);
        const auto center = CascadeShaderManager::GetFrustumCenter(corners);
        const auto tar = center + forward;
        const auto view = Matrix::CreateLookAt(center, tar, sceneComponent->GetUp());
        const auto proj = CascadeShaderManager::GetOrthographicProjByCorners(corners, view);
        eyeData.mView = view;
        eyeData.mProj = proj;
    }
}


void DirectionalLightComponent::Draw()
{
    const bool clear = drawFrustum && drawCascadeBox && engInst->GetInputDevice()->IsKeyDown(Keys::N);

    const bool doDrawDebug = drawFrustum && drawCascadeBox
                             && (!(engInst->GetInputDevice()->IsKeyDown(Keys::LeftShift)
                                   || engInst->GetInputDevice()->IsKeyDown(Keys::RightShift))
                                 && engInst->GetInputDevice()->IsKeyDown(Keys::M));
    if (doDrawDebug || clear) frustums.clear();

    const auto camData = engInst->GetCurCamera()->GetEyeData();
    if (engInst->useCascadeShadow)
    {
        float percentDist = 1.0f / static_cast<float>(CASCADE_COUNT);
        for (uint32_t i = 0; i < CASCADE_COUNT; ++i)
        {
            Matrix subProj;
            float dist;
            engInst->GetCurCamera()->GetProjAndDistanceBySection(percentDist * i, percentDist * static_cast<float>(i + 1), subProj,
                dist);
            auto corners = CascadeShaderManager::GetFrustumCorners(camData.mView, subProj);
            if (doDrawDebug && !clear)
                frustums.insert(corners);

        }
    }

    else
    {
        Matrix subProj;
        float dist;
        engInst->GetCurCamera()->GetProjAndDistanceBySection(0.0f, 0.2f, subProj, dist);
        auto corners = CascadeShaderManager::GetFrustumCorners(camData.mView, subProj);
        if (drawFrustum && drawCascadeBox && engInst->GetInputDevice()->IsKeyDown(Keys::M)) frustums.insert(corners);

        engInst->GetCurCamera()->GetProjAndDistanceBySection(0.2f, 0.4f, subProj, dist);
        corners = CascadeShaderManager::GetFrustumCorners(camData.mView, subProj);
        if (doDrawDebug && !clear)
            frustums.insert(corners);
    }

    for (auto& corners : frustums)
    {
        DrawDebugBoxes(corners);
    }
    //LightComponent::Draw();
}

void DirectionalLightComponent::DrawDebugBoxes(Array<Vector4> corners)
{
    Array<VertexNoTex> vertexes;
    for (const auto& corn : corners)
    {
        vertexes.insert(VertexNoTex{corn, Vector4::One});
    }
    drawFrustum->GetDrawBox()->GetDrawComponent()->SetVertices(vertexes);
    drawFrustum->Draw();
    vertexes.clear();
    const auto center = CascadeShaderManager::GetFrustumCenter(corners);
    const auto tar = center + sceneComponent->GetForward();
    const auto view = Matrix::CreateLookAt(center, tar, sceneComponent->GetUp());
    const auto proj = CascadeShaderManager::GetOrthographicProjByCorners(corners, view);
    corners.clear();
    corners = CascadeShaderManager::GetFrustumCorners(view, proj);
    for (const auto& corn : corners)
    {
        vertexes.insert(VertexNoTex{corn, drawCascadeBox->GetDrawBox()->initColor});
    }
    drawCascadeBox->GetDrawBox()->GetDrawComponent()->SetVertices(vertexes);
    drawCascadeBox->Draw();
}

void DirectionalLightComponent::SetCurrentCascadeData(uint32_t idx)
{
    LightComponent::SetCurrentCascadeData(idx);
    eyeData = eyesData[idx];
}
