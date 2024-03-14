#include "SceneComponent.h"

#include "../Core/Game.h"
#include "../Game/Camera.h"
#include "../Utils/Types.h"

SceneComponent::SceneComponent()
{
    game = &Game::GetGame();
}

void SceneComponent::Initialize()
{
    transform.location = initPosition;
    transform.rotate = initRotation;
    transform.scale = initScale;
    UpdateTransformMatrix();

    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(ViewData);

    game->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
    buffers.insert(game->GetIdxCurrentPipeline(), constantBuffer);
}

void SceneComponent::Draw()
{
    constantBuffer = buffers[game->GetIdxCurrentPipeline()];
    game->GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
}

void SceneComponent::Reload()
{
    GameComponent::Reload();
    transform.location = initPosition;
    transform.rotate = initRotation;
    transform.scale = initScale;
}

void SceneComponent::UpdateTransformMatrix()
{
    const float pitch = RadiansFromDegree(transform.rotate.x);
    const float yaw = RadiansFromDegree(transform.rotate.y);
    const float roll = RadiansFromDegree(transform.rotate.z);
    mTransform = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
    mTransform.Translation(GetLocation());
    mTransform *= mTransform.CreateScale(GetScale());
}

void SceneComponent::Update(float timeTick)
{
    constantBuffer = buffers[game->GetIdxCurrentPipeline()];
    UpdateTransformMatrix();
    if (!game->GetContext())
    {
        printf("PENIS");
    }
    UpdateConstantBuffer();
}

void SceneComponent::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE res = {};
    game->GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    auto Res = GetWorldMatrix();
    /*printf(" Position: posX=%04.4f posY:%04.4f posZ:%04.4f\n",
        Res.Translation().x,
        Res.Translation().y,
        Res.Translation().z);*/

    ViewData data =
    {
        Res.Transpose(),
        //Matrix::CreateWorld(Res.Translation(), Res.Forward(), Res.Up()).Transpose(),
        Matrix(game->GetCamera()->mView).Transpose(),
        Matrix(game->GetCamera()->mProj).Transpose(),
    };

    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &data, sizeof(ViewData));

    game->GetContext()->Unmap(constantBuffer, 0);
}

void SceneComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    constantBuffer->Release();
}

const Vector3& SceneComponent::GetForward() const
{
    const float pitch = RadiansFromDegree(transform.rotate.x);
    const float yaw = RadiansFromDegree(transform.rotate.y);
    const float roll = RadiansFromDegree(transform.rotate.z);
    return Matrix::CreateFromYawPitchRoll(yaw, pitch, roll).Forward();
}

const Vector3& SceneComponent::GetRight() const
{
    const float pitch = RadiansFromDegree(transform.rotate.x);
    const float yaw = RadiansFromDegree(transform.rotate.y);
    const float roll = RadiansFromDegree(transform.rotate.z);
    return Matrix::CreateFromYawPitchRoll(yaw, pitch, roll).Right();
}

const Vector3& SceneComponent::GetUp() const
{
    const float pitch = RadiansFromDegree(transform.rotate.x);
    const float yaw = RadiansFromDegree(transform.rotate.y);
    const float roll = RadiansFromDegree(transform.rotate.z);
    return Matrix::CreateFromYawPitchRoll(yaw, pitch, roll).Up();
}

const Vector3& SceneComponent::GetGlobalUp() const
{
    return Vector3(0.0f, 1.0f, 0.0f);
}

const Matrix& SceneComponent::GetWorldMatrix()
{
    SceneComponent* rootComp = this;
    Matrix Res = rootComp->mTransform;

    SceneComponent* parentComp = rootComp->parentComponent;
    while (parentComp)
    {
        if (rootComp->attachOnlyTranslation)
        {
            Res *= Matrix::CreateTranslation(parentComp->mTransform.Translation());
            //return Res;
        }
        else
            Res *= parentComp->mTransform;
        
        rootComp = parentComp;
        parentComp = rootComp->parentComponent;
    }

    return Res;
}
