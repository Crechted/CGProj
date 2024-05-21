#pragma once
#include <d3d11.h>

#include "LightComponent.h"
#include "SimpleMath.h"
#include "Core/Engine.h"

class PostRenderImage;
class DrawBoxComponent;

using namespace DirectX::SimpleMath;

/*struct DirectionLightData
{
    Vector4 direction;
    Vector4 color;
    Vector4 kaSpecPowKsX;
    Matrix mShadowTransform;
};*/

struct CascadeData
{
    Matrix ViewProj[CASCADE_COUNT];
    float Distances[CASCADE_COUNT];
};

struct CascadeIndex
{
    alignas(16) uint32_t idx;
};

class DirectionalLightComponent : public LightComponent
{
public:
    DirectionalLightComponent();
    void Initialize() override;
    void DestroyResource() override;
    void UpdateSubresource() override;
    void UpdateShaderResources() override;
    void DrawDebugBoxes(Array<Vector4> corners);
    void Update(float timeTick) override;
    void Draw() override;
    void SetCurrentCascadeData(uint32_t idx) override;

protected:
    Buffer* cascadeBuffer = nullptr;
    CascadeData cascData;
    Array<EyeViewData> eyesData;

    Array<PostRenderImage*> cascadeImgs;
    Array<Buffer*> addBufIndexes;

    Array<Array<Vector4>> frustums;
    DrawBoxComponent* drawFrustum;
    DrawBoxComponent* drawCascadeBox;

    void CreateCascadeImage();
};
