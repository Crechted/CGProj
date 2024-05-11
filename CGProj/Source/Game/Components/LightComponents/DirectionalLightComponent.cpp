#include "DirectionalLightComponent.h"

#include "Game/Components/DrawBoxComponent.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Components/DrawComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Objects/Mesh.h"
#include "Game/Objects/DrawBox.h"


DirectionalLightComponent::DirectionalLightComponent()
{
    drawFrustum = CreateComponent<DrawBoxComponent>(Vector3::Zero, Vector3::Zero, Vector4::One, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    drawCascadeBox = CreateComponent<DrawBoxComponent>(Vector3::Zero, Vector3::Zero, Vector4(1.0f, 0.0f, 0.0f, 1.0f),
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void DirectionalLightComponent::Initialize()
{
    if (engInst->useCascadeShadow)
    {
        D3D11_BUFFER_DESC constBufDesc;
        constBufDesc.Usage = D3D11_USAGE_DEFAULT;
        constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constBufDesc.CPUAccessFlags = 0;
        constBufDesc.MiscFlags = 0;
        constBufDesc.StructureByteStride = 0;
        constBufDesc.ByteWidth = sizeof(CascadeData);
        engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &cascadeBuffer);
    }

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
}

void DirectionalLightComponent::DestroyResource()
{
    LightComponent::DestroyResource();
    if (cascadeBuffer) cascadeBuffer->Release();
}

void DirectionalLightComponent::UpdateSubresource()
{
    LightComponent::UpdateSubresource();

    if (engInst->useCascadeShadow)
    {
        engInst->GetContext()->UpdateSubresource(cascadeBuffer, 0, nullptr, &cascData, 0, 0);
        engInst->GetContext()->GSSetConstantBuffers(3, 1, &cascadeBuffer);
        engInst->GetContext()->PSSetConstantBuffers(3, 1, &cascadeBuffer);
    }
}

void DirectionalLightComponent::UpdateShaderResources()
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
    if (drawFrustum && drawCascadeBox && engInst->GetInputDevice()->IsKeyDown(Keys::M)) frustums.clear();

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
            if (drawFrustum && drawCascadeBox && engInst->GetInputDevice()->IsKeyDown(Keys::M)) frustums.insert(corners);
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
        if (drawFrustum && drawCascadeBox && engInst->GetInputDevice()->IsKeyDown(Keys::M)) frustums.insert(corners);

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
