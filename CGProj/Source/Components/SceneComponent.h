#pragma once
#include <d3d11.h>

#include "GameComponent.h"
#include "../Utils/Array/Array.h"
#include "Inc/SimpleMath.h"


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
    SceneComponent();
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
    void SetTransform(const Transform& trans) { transform = trans; }

    const Vector3& GetLocation() const { return transform.location; }
    void SetLocation(const Vector3& loc) { transform.location = loc; }
    void AddLocation(const Vector3& addLoc) { transform.location += addLoc; }

    const Vector3& GetScale() const { return transform.scale; }
    void SetScale(const Vector3& scale) { transform.scale = scale; }

    const Vector3& GetRotation() const { return transform.rotate; }
    void SetRotation(const Vector3& rot) { transform.rotate = rot; }
    void AddRotation(const Vector3& addRot) { transform.rotate += addRot; }


    const Vector3& GetForward() const;
    const Vector3& GetRight() const;
    const Vector3& GetUp() const;

    const Vector3& GetGlobalUp() const;

    const Matrix& GetWorldMatrix();

    void AttachTo(SceneComponent* par) {parentComponent = par;}

    Vector3 initPosition = Vector3(0.0f);
    Vector3 initRotation = Vector3(0.0f);
    Vector3 initScale = Vector3(1.0f);

protected:    
    SceneComponent* parentComponent;
    Transform transform;
    Matrix mTransform;
    
    Array<ID3D11Buffer*> buffers;
    ID3D11Buffer* constantBuffer;
    D3D11_BUFFER_DESC constBufDesc;

};
