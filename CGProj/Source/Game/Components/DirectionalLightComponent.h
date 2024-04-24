#pragma once
#include <d3d11.h>

#include "LightComponent.h"
#include "SimpleMath.h"
#include "Core/Components/SceneComponent.h"
using namespace DirectX::SimpleMath;

struct DirectionLightData
{
    Vector4 direction;
    Vector4 color;
    Vector4 kaSpecPowKsX;
};

class DirectionalLightComponent : public LightComponent
{
public:
    void Initialize() override;
    void DestroyResource() override;
    DirectionLightData GetLightData() const {return lightData;}
    Transform GetTransform() const {return transform;}
    void UpdateSubresource(DirectionLightData Data);
    void Update(float timeTick) override;
    void Draw() override;
    
protected:
    Transform transform;
    DirectionLightData lightData;
    ID3D11Buffer* lightBuffer = nullptr;
    float ambietKoeff = 0.1f;
    float specPow = 50.0f;
    float specKoeff = 0.25f;

};
