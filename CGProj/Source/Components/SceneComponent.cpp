#include "SceneComponent.h"

#include "../Core/Game.h"

SceneComponent::SceneComponent()
{
    game = &Game::GetGame();
}

void SceneComponent::Initialize()
{
    initPosition = Vector3(2.0f, 2.0f, 2.0f);
    forward = Vector3(-1.0, -0.2, -1.0);
    right = Vector3(0.0, 1.0, 0.0);
    up = Vector3(0.0, 1.0, 0.0);

    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(ViewData);

    game->device->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
}

void SceneComponent::Draw()
{
    game->context->VSSetConstantBuffers(0, 1, &constantBuffer);
}

void SceneComponent::Reload()
{
    GameComponent::Reload();
}

void SceneComponent::Update(float timeTick)
{
    UpdateConstantBuffer();
}

void SceneComponent::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE res = {};
    game->context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    ViewData data =
    {
        //Matrix(game->camera->mWorld).Transpose(),
        GetWorldMatrix().Transpose(),
        Matrix(game->camera->mView).Transpose(),
        Matrix(game->camera->mProj).Transpose(),
    };

    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &data, sizeof(ViewData));

    game->context->Unmap(constantBuffer, 0);
}

void SceneComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    constantBuffer->Release();
}

Vector3 SceneComponent::Forward() const
{
    //return forward;
    return mTransform.Translation() + mTransform.Forward();
}

Matrix SceneComponent::GetWorldMatrix()
{
    SceneComponent* rootComponent = this;
    Matrix Res = {1.f, 0.f, 0.f, 0.f,
                  0.f, 1.f, 0.f, 0.f,
                  0.f, 0.f, 1.f, 0.f,
                  0.f, 0.f, 0.f, 1.f};
    while (rootComponent)
    {
        Res *= rootComponent->mTransform;
        rootComponent = parentComponent;    
    }
    return Res;
}
