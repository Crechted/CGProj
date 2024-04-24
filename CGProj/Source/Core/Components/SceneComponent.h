#pragma once
#include <d3d11.h>

#include "GameComponent.h"
#include "SimpleMath.h"
#include "Utils/Array/Array.h"

using namespace DirectX::SimpleMath;


struct ViewData
{
    Matrix mWorld;
    Matrix mView;
    Matrix mProj;
    Matrix mShadowTransform;
    Vector4 objPos;
    Vector4 camPos;
};

struct Transform
{
    Vector3 GetUp() const;
    Vector3 GetForward() const;
    Vector3 GetRight() const;
    Matrix GetMatrix() const;
    Vector3 location;
    Vector3 rotate;
    Vector3 scale;
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
    void SetTransform(const Transform& trans) { transform = trans; }
    void SetWorldTransform(const Transform& trans);

    const Vector3& GetLocation() const { return transform.location; }
    Vector3 GetWorldLocation() const;
    void SetLocation(const Vector3& loc) { transform.location = loc; }
    void SetWorldLocation(const Vector3& loc);
    void AddLocation(const Vector3& addLoc) { transform.location += addLoc; }

    const Vector3& GetScale() const { return transform.scale; }
    void SetScale(const Vector3& scale) { transform.scale = scale; }

    const Vector3& GetRotation() const { return transform.rotate; }
    Vector3 GetWorldRotation() const;
    void SetRotation(const Vector3& rot) { transform.rotate = rot; }
    void AddRotation(const Vector3& addRot) { transform.rotate += addRot; }
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
    bool playVertAnim = false;
    
    SceneComponent* GetParentComponent() const { return parentComponent; }

    static Transform TransformFromMatrix(Matrix& mTrans);
    static Matrix MatrixFromTransform(const Transform& mTrans);

protected:
    SceneComponent* parentComponent = nullptr;
    Transform transform;
    Matrix mTransform;

    float ambietKoeff = 0.1f;
    float specPow = 50.0f;
    float specKoeff = 0.25f;

    //Array<BufStruct> buffers;
    ID3D11Buffer* viewBuffer = nullptr;
    D3D11_BUFFER_DESC constBufDesc;

};
