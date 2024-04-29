#pragma once
#include "SimpleMath.h"
#include "Core/Shader.h"
#include "Core/Objects/Object.h"

class RenderTarget;
class Shader;

struct PostProcessVertex
{
    DirectX::SimpleMath::Vector4 pos;
    DirectX::SimpleMath::Vector2 tex;
};

class PostProcess : public Object
{
public:
    PostProcess(int32_t screenW, int32_t screenH, LPCWSTR shaderPath = L"./Resource/Shaders/PostProcessShader.hlsl");
    void Draw() override;
    void CreateShader();
    void Initialize() override;
    void Destroy() override;

    void SetSRV(ID3D11ShaderResourceView* srv);
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    RenderTarget* GetRenderTarget() const { return renderTarget; }

protected:
    Shader* shader = nullptr;
    ID3D11ShaderResourceView* texSRV;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    Array<PostProcessVertex> vertices;
    Array<int32_t> indexes;

    RenderTarget* renderTarget;
    int32_t screenWidth;
    int32_t screenHeight;
    LPCWSTR shaderPath = L"./Resource/Shaders/PostProcessShader.hlsl";
};
