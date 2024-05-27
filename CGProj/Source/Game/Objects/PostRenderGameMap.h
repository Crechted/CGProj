#pragma once
#include "PostRenderImage.h"

class RenderTarget;
class Camera;

class PostRenderGameMap : public PostRenderImage
{
public:
    PostRenderGameMap(const LPCWSTR shaderPath = L"./Resource/Shaders/ImgShader.hlsl", ID3D11ShaderResourceView* imageSRV = nullptr,
        Vector2 viewportDim = Vector2(800, 800),
        Vector2 viewportPos = Vector2::Zero, Vector2 viewportDepth = Vector2(0.0f, 1.0f));

    void Destroy() override;
    void Update(float timeTick) override;
    void Initialize() override;
    void Draw() override;

    ID3D11ShaderResourceView* GetMapSRV() const;

protected:
    Camera* mapCamera = nullptr;
    RenderTarget* mapRenderTarget = nullptr;

};
