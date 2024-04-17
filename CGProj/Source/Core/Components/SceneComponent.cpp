#include "SceneComponent.h"

#include "Core/Engine.h"
#include "Game/Camera.h"
#include "Utils/Types.h"

SceneComponent::SceneComponent(SceneComponent* parentComp, Vector3 position, Vector3 rotation, Vector3 scale)
    : parentComponent(parentComp), initPosition(position), initRotation(rotation), initScale(scale)
{
}

SceneComponent::SceneComponent(Transform transf, SceneComponent* parentComp) : parentComponent(parentComp), initPosition(transf.location), initRotation(transf.rotate), initScale(transf.rotate)
{
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

    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &constantBuffer);
    buffers.insert(engInst->GetIdxCurrentPipeline(), constantBuffer);
    GameComponent::Initialize();
}

void SceneComponent::PreDraw()
{
    GameComponent::PreDraw();
    constantBuffer = buffers[engInst->GetIdxCurrentPipeline()];
    //UpdateTransformMatrix();
    UpdateConstantBuffer();
    engInst->GetContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
}

void SceneComponent::Draw()
{
    GameComponent::Draw();
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
    const auto rad = RadiansFromDegree(transform.rotate);
    mTransform = Matrix::CreateFromYawPitchRoll(rad);
    mTransform.Translation(GetLocation());
    mTransform *= mTransform.CreateScale(GetScale());
}

void SceneComponent::Update(float timeTick)
{
    GameComponent::Update(timeTick);
    constantBuffer = buffers[engInst->GetIdxCurrentPipeline()];
    UpdateTransformMatrix();
    //UpdateConstantBuffer();
}

void SceneComponent::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE res = {};
    engInst->GetContext()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    auto Res = GetWorldMatrix();
    /*printf(" Position: posX=%04.4f posY:%04.4f posZ:%04.4f\n",
        Res.Translation().x,
        Res.Translation().y,
        Res.Translation().z);*/

    ViewData data =
    {
        Res.Transpose(),
        Matrix(engInst->GetCurCamera()->mView).Transpose(),
        Matrix(engInst->GetCurCamera()->mProj).Transpose(),
    };

    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &data, sizeof(ViewData));

    engInst->GetContext()->Unmap(constantBuffer, 0);
}

void SceneComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (constantBuffer) constantBuffer->Release();
}

const Vector3& SceneComponent::GetWorldLocation() const
{
    return GetWorldMatrix().Translation();
}

const Vector3& SceneComponent::GetWorldRotation() const
{
    const auto euler = GetWorldMatrix().ToEuler();
    const auto deg = DegreeFromRadians(euler);
    return deg;
}

void SceneComponent::AddQuatRotation(const Vector3& addRot)
{
    const auto newRot = GetRotation() + addRot;    
    auto quat = Quaternion::CreateFromRotationMatrix(mTransform);
    quat = quat*Quaternion::CreateFromYawPitchRoll(RadiansFromDegree(addRot));
    const auto euler = quat.ToEuler();
    const auto deg = DegreeFromRadians(euler);
    transform.rotate = deg;
}

const Vector3& SceneComponent::GetForward() const
{
    const auto rad = RadiansFromDegree(transform.rotate);
    return Matrix::CreateFromYawPitchRoll(rad).Forward();
}

const Vector3& SceneComponent::GetRight() const
{
    const auto rad = RadiansFromDegree(transform.rotate);
    return Matrix::CreateFromYawPitchRoll(rad).Right();
}

const Vector3& SceneComponent::GetUp() const
{
    const auto rad = RadiansFromDegree(transform.rotate);
    return Matrix::CreateFromYawPitchRoll(rad).Up();
}

const Vector3& SceneComponent::GetGlobalUp() const
{
    return Vector3(0.0f, 1.0f, 0.0f);
}

Matrix SceneComponent::GetWorldMatrix() const
{
    const SceneComponent* rootComp = this;
    Matrix Res = rootComp->mTransform;

    const SceneComponent* parentComp = rootComp->parentComponent;
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
