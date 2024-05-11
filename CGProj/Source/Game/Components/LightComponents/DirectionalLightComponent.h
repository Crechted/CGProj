#pragma once
#include <d3d11.h>

#include "LightComponent.h"
#include "SimpleMath.h"
#include "Core/Engine.h"

class DrawBoxComponent;

using namespace DirectX::SimpleMath;

struct DirectionLightData
{
    Vector4 direction;
    Vector4 color;
    Vector4 kaSpecPowKsX;
    Matrix mShadowTransform;
};

struct CascadeData
{
    Matrix ViewProj[CASCADE_COUNT];
    float Distances[CASCADE_COUNT];
};

class DirectionalLightComponent : public LightComponent
{
public:
    DirectionalLightComponent();
    void Initialize() override;
    void DestroyResource() override;
    DirectionLightData GetLightData() const { return lightData; }
    void UpdateSubresource(DirectionLightData Data);
    void UpdateShaderResources();
    void DrawDebugBoxes(Array<Vector4> corners);
    void Update(float timeTick) override;
    void Draw() override;
    void SetCurrentCascadeData(uint32_t idx) override;
    
protected:
    DirectionLightData lightData;
    ID3D11Buffer* lightBuffer = nullptr;
    ID3D11Buffer* cascadeBuffer = nullptr;
    float ambietKoeff = 0.1f;
    float specPow = 50.0f;
    float specKoeff = 0.25f;    
    CascadeData cascData;
    Array<EyeViewData> eyesData;

    Array<Array<Vector4>> frustums;
    DrawBoxComponent* drawFrustum;
    DrawBoxComponent* drawCascadeBox;
};
