#pragma once
#include "Core/CoreTypes.h"
#include "Core/Objects/PostRenderObject.h"

class Buffer;
class Shader;

class PostRenderImage : public PostRenderObject
{
public:
    PostRenderImage(ID3D11ShaderResourceView* imageSRV = nullptr, Vector2 viewportDim = Vector2(800, 800),
        Vector2 viewportPos = Vector2::Zero, Vector2 viewportDepth = Vector2(0.0f, 1.0f));
    PostRenderImage(ID3D11ShaderResourceView* imageSRV, const D3D11_VIEWPORT& viewport);

    void SetSRV(ID3D11ShaderResourceView* SRV) { imageSRV = SRV ? SRV : imageSRV; }
    void SetShaderPath(const LPCWSTR path) { shaderPath = path; }
    ID3D11ShaderResourceView* GetSRV() const { return imageSRV; }
    void Destroy() override;
    void Initialize() override;
    void Draw() override;

protected:
    ID3D11ShaderResourceView* imageSRV = nullptr;
    Shader* imgShader = nullptr;
    LPCWSTR shaderPath = L"./Resource/Shaders/FullQuadShader.hlsl";
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    Buffer* vertBuf = nullptr;
    Buffer* indBuf = nullptr;
    void CreateBuffers();
};
