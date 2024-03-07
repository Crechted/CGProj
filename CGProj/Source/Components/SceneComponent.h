#pragma once
#include <d3d11.h>

#include "GameComponent.h"
#include "../Utils/Array/Array.h"
#include "Inc/SimpleMath.h"


struct ViewData
{
    DirectX::SimpleMath::Matrix mWorld;
    DirectX::SimpleMath::Matrix mView;
    DirectX::SimpleMath::Matrix mProj;
};

using namespace DirectX::SimpleMath;

class SceneComponent : public GameComponent
{
public:
    SceneComponent();
    SceneComponent* parentComponent;
    Array<SceneComponent*> childComponents;
    
    void Initialize() override;
    void Draw() override;
    void Reload() override;
    void UpdateConstantBuffer();
    void Update(float timeTick) override;
    void DestroyResource() override;

    //Vector3 GetLocation() const { return location; }
    Vector3 GetLocation() const { return mTransform.Translation(); }
    Vector3 Forward() const ;
    Vector3 Right() const {return mTransform.Right();}
    //Vector3 Up() const {return up;}
    Vector3 Up() const {return mTransform.Up();}


    Matrix GetWorldMatrix();
    Matrix mTransform;
    Vector3 forward = Vector3(1.0, 0.0, 0.0);
    Vector3 right = Vector3(0.0, 1.0, 0.0);
    Vector3 up = Vector3(1.0, 1.0, 1.0);

    Vector3 location;

    Vector3 rotation;

    Vector3 initPosition = Vector3(0.0f);
    Vector3 initRotation = Vector3(0.0f);

    bool isAttachedToParent = false;
    
    ID3D11Buffer* constantBuffer;
    D3D11_BUFFER_DESC constBufDesc;
    
};
