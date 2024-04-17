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
};

struct Transform
{
    Vector3 location;
    Vector3 rotate;
    Vector3 scale;
};

class SceneComponent : public GameComponent
{
public:
    SceneComponent(SceneComponent* parentComp = nullptr, Vector3 position = Vector3(0.0f), Vector3 rotation = Vector3(0.0f), Vector3 scale = Vector3(1.0f));
    SceneComponent(Transform transf, SceneComponent* parentComp = nullptr);
    /*Array<SceneComponent*> childComponents;*/

    void Initialize() override;
    void PreDraw() override;
    void Draw() override;
    void Reload() override;
    void UpdateTransformMatrix();
    void UpdateConstantBuffer();
    void Update(float timeTick) override;
    void DestroyResource() override;

    //Vector3 GetLocation() const { return location; }
    const Transform& GetTransform() const { return transform; }
    void SetTransform(const Transform& trans) { transform = trans; }

    const Vector3& GetLocation() const { return transform.location; }
    const Vector3& GetWorldLocation() const;
    void SetLocation(const Vector3& loc) { transform.location = loc; }
    void AddLocation(const Vector3& addLoc) { transform.location += addLoc; }

    const Vector3& GetScale() const { return transform.scale; }
    void SetScale(const Vector3& scale) { transform.scale = scale; }

    const Vector3& GetRotation() const { return transform.rotate; }
    const Vector3& GetWorldRotation() const;
    void SetRotation(const Vector3& rot) { transform.rotate = rot; }
    void AddRotation(const Vector3& addRot) { transform.rotate += addRot; }
    void AddQuatRotation(const Vector3& addRot);


    const Vector3& GetForward() const;
    const Vector3& GetRight() const;
    const Vector3& GetUp() const;

    const Vector3& GetGlobalUp() const;

    Matrix GetWorldMatrix() const;

    void AttachTo(SceneComponent* par) { parentComponent = par; }

    Vector3 initPosition = Vector3(0.0f);
    Vector3 initRotation = Vector3(0.0f);
    Vector3 initScale = Vector3(1.0f);

    bool attachOnlyTranslation = false;

    SceneComponent* GetParentComponent() const { return parentComponent; }

protected:
    SceneComponent* parentComponent = nullptr;
    Transform transform;
    Matrix mTransform;

    Array<ID3D11Buffer*> buffers;
    ID3D11Buffer* constantBuffer = nullptr;
    D3D11_BUFFER_DESC constBufDesc;
};
