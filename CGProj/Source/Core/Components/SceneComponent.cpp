#include "SceneComponent.h"

#include "Core/Engine.h"
#include "Game/Camera.h"
#include "Utils/Types.h"

SceneComponent::SceneComponent(SceneComponent* parentComp, Vector3 position, Vector3 rotation, Vector3 scale)
    : parentComponent(parentComp), initPosition(position), initRotation(rotation), initScale(scale)
{
}

SceneComponent::SceneComponent(Transform transf, SceneComponent* parentComp)
    : parentComponent(parentComp), initPosition(transf.location), initRotation(transf.rotate), initScale(transf.rotate)
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

    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &viewBuffer);
    constBufDesc.ByteWidth = sizeof(DirectionLightData);
    engInst->GetDevice()->CreateBuffer(&constBufDesc, nullptr, &lightBuffer);
    buffers.insert(engInst->GetIdxCurrentPipeline(), {viewBuffer, lightBuffer});
    GameComponent::Initialize();
}

void SceneComponent::PreDraw()
{
    GameComponent::PreDraw();
    const auto buffs = buffers[engInst->GetIdxCurrentPipeline()];
    viewBuffer = buffs.viewBuffer;
    lightBuffer = buffs.lightBuffer;
    //UpdateTransformMatrix();
    UpdateConstantBuffer();
    ID3D11Buffer* psBuffers[] = {viewBuffer, lightBuffer};
    engInst->GetContext()->VSSetConstantBuffers(0, 1, &viewBuffer);
    engInst->GetContext()->PSSetConstantBuffers(0, 2, psBuffers);
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
    mTransform = MatrixFromTransform(transform);
}

void SceneComponent::Update(float timeTick)
{
    GameComponent::Update(timeTick);
    const auto buffs = buffers[engInst->GetIdxCurrentPipeline()];
    viewBuffer = buffs.viewBuffer;
    lightBuffer = buffs.lightBuffer;
    UpdateTransformMatrix();
    //UpdateConstantBuffer();
}

void SceneComponent::UpdateConstantBuffer()
{
    D3D11_MAPPED_SUBRESOURCE res = {};

    auto worldMat = GetWorldMatrix();
    /*printf(" Position: posX=%04.4f posY:%04.4f posZ:%04.4f\n",
        Res.Translation().x,
        Res.Translation().y,
        Res.Translation().z);*/

    const auto cam = engInst->GetCurCamera();
    const auto camLoc = cam->GetSceneComponent()->GetWorldLocation();
    ViewData viewData =
    {
        worldMat.Transpose(),
        Matrix(cam->mView).Transpose(),
        Matrix(cam->mProj).Transpose(),
        Vector4(worldMat.Translation().x, worldMat.Translation().y, worldMat.Translation().z, 1.0f),
        Vector4(camLoc.x, camLoc.y, camLoc.z, 1.0f),
    };

    Vector3 rot(-90.0f, 0.0f, 0.0f);
    DirectionLightData lightData;
    lightData.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    //lightData.direction = Vector4::Transform(Vector4(-3.0f, -2.0f, -0.5f, 0.0f), Quaternion::CreateFromYawPitchRoll(RadiansFromDegree(rot)));
    lightData.direction = Vector4(-1.0f, -1.0f, 0.0f, 0.0f);
    lightData.direction.Normalize();
    ambietKoeff = 0.1f;
    specPow = 50.0f;
    specKoeff = 0.25f;
    lightData.kaSpecPowKsX = Vector4{ambietKoeff, specPow, specKoeff, 0.0f};

    engInst->GetContext()->Map(viewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);        
    auto dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &viewData, sizeof(viewData));
    engInst->GetContext()->Unmap(viewBuffer, 0);
    
    
    engInst->GetContext()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);        
    dataPtr = reinterpret_cast<float*>(res.pData);
    memcpy(dataPtr, &lightData, sizeof(lightData));
    engInst->GetContext()->Unmap(lightBuffer, 0);
    
    //engInst->GetContext()->UpdateSubresource(viewBuffer, 0, nullptr, &viewData, 0, 0);
    //engInst->GetContext()->UpdateSubresource(lightBuffer, 0, nullptr, &lightData, 0, 0);
}

void SceneComponent::DestroyResource()
{
    GameComponent::DestroyResource();
    if (viewBuffer) viewBuffer->Release();
}

Transform SceneComponent::GetWorldTransform() const
{
    auto worldMat = GetWorldMatrix();
    return TransformFromMatrix(worldMat);
}

void SceneComponent::SetWorldTransform(const Transform& trans)
{
    transform = GetLocalFromWorldTransform(trans);
}

Vector3 SceneComponent::GetWorldLocation() const
{
    return GetWorldMatrix().Translation();
}

void SceneComponent::SetWorldLocation(const Vector3& loc)
{
    Transform trans;
    trans.location = loc;
    transform.location = GetLocalFromWorldTransform(trans).location;
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
    quat = quat * Quaternion::CreateFromYawPitchRoll(RadiansFromDegree(addRot));
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
    Array<Matrix> compos;
    return GetWorldMatrixAndMatComposition(compos);
}

Matrix SceneComponent::GetWorldMatrixAndMatComposition(Array<Matrix>& compos) const
{
    const SceneComponent* rootComp = this;
    Matrix Res = rootComp->mTransform;

    SceneComponent* parentComp = rootComp->parentComponent;
    while (parentComp)
    {
        Matrix elementCompos;
        if (rootComp->attachTranslation && rootComp->attachRotate && rootComp->attachScale)
        {
            elementCompos *= parentComp->mTransform;
        }
        else if (rootComp->attachTranslation && rootComp->attachRotate)
        {
            elementCompos.Translation(parentComp->mTransform.Translation());
            elementCompos *= elementCompos.CreateFromYawPitchRoll(parentComp->mTransform.ToEuler());
        }
        else if (rootComp->attachTranslation && rootComp->attachScale)
        {
            elementCompos.Translation(parentComp->mTransform.Translation());
            elementCompos *= elementCompos.CreateScale(parentComp->transform.scale);
        }
        else if (rootComp->attachRotate && rootComp->attachScale)
        {
            elementCompos *= elementCompos.CreateFromYawPitchRoll(parentComp->mTransform.ToEuler());
            elementCompos *= elementCompos.CreateScale(parentComp->transform.scale);
        }
        else if (rootComp->attachTranslation)
        {
            elementCompos.Translation(parentComp->mTransform.Translation());
        }
        else if (rootComp->attachRotate)
        {
            elementCompos *= elementCompos.CreateFromYawPitchRoll(parentComp->mTransform.ToEuler());
        }
        else if (rootComp->attachScale)
        {
            elementCompos *= elementCompos.CreateScale(parentComp->transform.scale);
        }

        Res *= elementCompos;
        compos.insert(elementCompos);
        rootComp = parentComp;
        parentComp = rootComp->parentComponent;
    }

    return Res;
}

Transform SceneComponent::GetLocalFromWorldTransform(const Transform& trans) const
{
    Array<Matrix> compos;
    GetWorldMatrixAndMatComposition(compos);
    Matrix mTrans = MatrixFromTransform(trans);
    for (auto iter = compos.reverseIterator(); iter.hasNext(); iter.next())
    {
        mTrans *= iter.get().Invert();
    }
    return TransformFromMatrix(mTrans);
}

Transform SceneComponent::TransformFromMatrix(Matrix& mTrans)
{
    Transform resTrans;
    auto quat = Quaternion::CreateFromRotationMatrix(mTrans);
    mTrans.Decompose(resTrans.scale, quat, resTrans.location);
    resTrans.rotate = DegreeFromRadians(quat.ToEuler());
    return resTrans;
}

Matrix SceneComponent::MatrixFromTransform(const Transform& trans)
{
    const auto rad = RadiansFromDegree(trans.rotate);
    Matrix resMat;
    resMat *= resMat.CreateScale(trans.scale);
    resMat *= Matrix::CreateFromYawPitchRoll(rad);
    resMat.Translation(trans.location);
    return resMat;
}
