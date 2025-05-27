#pragma once
#include <d3d11.h>
#include <forward_list>

#include "GameComponent.h"
#include "SimpleMath.h"
#include "Utils/Array/Array.h"

using namespace DirectX::SimpleMath;


struct ViewData
{
    Matrix mWorld;
    Matrix mViewProj;
    Matrix mView;
    Matrix mProj;
    Vector4 objPos;
    Vector4 camPos;
};

struct Transform
{
    Transform(Vector3 loc = Vector3::Zero, Vector3 rot = Vector3::Zero, Vector3 scale = Vector3::One)
        : location(loc), rotate(rot), scale(scale)
    {
    }

    Transform(const Transform& transform) = default;

    Transform(Transform&& transform)
        : location(transform.location), rotate(transform.rotate), scale(transform.scale)
    {
        transform = Identity;
    }

    bool operator==(const Transform& rhs) const
    {
        return location == rhs.location && rotate == rhs.rotate && scale == rhs.scale;
    }

    Transform& operator=(const Transform& other)
    {
        if (*this == other) return *this;
        Transform temp(other);
        std::swap(this->location, temp.location);
        std::swap(this->rotate, temp.rotate);
        std::swap(this->scale, temp.scale);
        return *this;
    }

    Transform& operator=(Transform&& other)
    {
        if (*this == other) return *this;

        std::swap(this->location, other.location);
        std::swap(this->rotate, other.rotate);
        std::swap(this->scale, other.scale);
        other = Identity;;
        return *this;
    }

    Vector3 GetUp() const;
    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Matrix GetMatrix() const;
    Vector3 location = Vector3::Zero;
    Vector3 rotate = Vector3::Zero;
    Vector3 scale = Vector3::One;

    static const Transform Identity;
};

struct BufStruct
{
    ID3D11Buffer* viewBuffer = nullptr;
    ID3D11Buffer* lightBuffer = nullptr;
};

class SceneComponent : public GameComponent
{
public:
    SceneComponent(SceneComponent* parentComp = nullptr, Vector3 position = Vector3(0.0f), Vector3 rotation = Vector3(0.0f),
        Vector3 scale = Vector3(1.0f));
    SceneComponent(Transform transf, SceneComponent* parentComp = nullptr);
    /*Array<SceneComponent*> childComponents;*/

    void Initialize() override;
    void Draw() override;
    void Reload() override;
    void UpdateTransformMatrix();
    void UpdateConstantBuffer();
    void Update(float timeTick) override;
    void DestroyResource() override;

    //Vector3 GetLocation() const { return location; }
    const Transform& GetTransform() const { return transform; }
    Transform GetWorldTransform() const;

    void SetTransform(const Transform& trans)
    {
        transform = trans;
        UpdateTransformMatrix();
    }

    void SetWorldTransform(const Transform& trans);

    const Vector3& GetLocation() const { return transform.location; }
    Vector3 GetWorldLocation() const;

    void SetLocation(const Vector3& loc)
    {
        transform.location = loc;
        UpdateTransformMatrix();
    }

    void SetWorldLocation(const Vector3& loc);

    void AddLocation(const Vector3& addLoc)
    {
        transform.location += addLoc;
        UpdateTransformMatrix();
    }

    const Vector3& GetScale() const { return transform.scale; }

    void SetScale(const Vector3& scale)
    {
        transform.scale = scale;
        UpdateTransformMatrix();
    }

    const Vector3& GetRotation() const { return transform.rotate; }
    Vector3 GetWorldRotation() const;

    void SetRotation(const Vector3& rot)
    {
        transform.rotate = rot;
        UpdateTransformMatrix();
    }

    void AddRotation(const Vector3& addRot)
    {
        transform.rotate += addRot;
        UpdateTransformMatrix();
    }

    void AddQuatRotation(const Vector3& addRot);


    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Vector3 GetUp() const;

    Vector3 GetGlobalUp() const;

    Matrix GetWorldMatrix() const;
    Matrix GetWorldMatrixAndMatComposition(Array<Matrix>& compos) const;
    Transform GetLocalFromWorldTransform(const Transform& trans) const;

    void AttachTo(SceneComponent* par) { parentComponent = par; }

    Vector3 initPosition = Vector3(0.0f);
    Vector3 initRotation = Vector3(0.0f);
    Vector3 initScale = Vector3(1.0f);

    bool attachTranslation = true;
    bool attachRotate = true;
    bool attachScale = true;

    SceneComponent* GetParentComponent() const { return parentComponent; }

    static Transform TransformFromMatrix(Matrix& mTrans);
    static Matrix MatrixFromTransform(const Transform& mTrans);
    float GetDistanceTo(const SceneComponent* otherScene) const;

protected:
    SceneComponent* parentComponent = nullptr;
    Transform transform;
    Matrix mTransform;

    //Array<BufStruct> buffers;
    ID3D11Buffer* viewBuffer = nullptr;
    D3D11_BUFFER_DESC constBufDesc;

};