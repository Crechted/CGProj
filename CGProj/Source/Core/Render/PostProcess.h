#pragma once
#include <d3d11.h>

#include "Core/CoreTypes.h"
#include "Core/Objects/Object.h"
//#include "Buffer.h"
#include "SimpleMath.h"

class RenderTarget;
class Shader;

struct ConstBufferData
{
    alignas(16) Vector2 time;
};

class PostProcess : public Object
{
public:
    PostProcess(int32_t screenW, int32_t screenH, LPCWSTR shaderPath = L"./Resource/Shaders/FullQuadShader.hlsl");
    void Update(float timeTick) override;
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
    ID3D11Buffer* constBuffer = nullptr;
    //Buffer<ConstBufferData>* constBuf = nullptr;
    
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;

    RenderTarget* renderTarget;
    int32_t screenWidth;
    int32_t screenHeight;
    LPCWSTR shaderPath = L"./Resource/Shaders/PostProcessShader.hlsl";
};
