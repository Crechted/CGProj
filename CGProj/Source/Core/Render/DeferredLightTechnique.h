#pragma once
#include "Utils/Array/Array.h"
#include <d3d11.h>

#include "Buffer.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
class LightComponent;
class RenderTarget;
class Engine;

struct LightIndexBuffer
{
    alignas(16) int32_t LightIndex;
};

struct ScreenToWorldParams
{
    Matrix InverseProjView;
    alignas(16) Vector3 CamPos;
    alignas(16) Vector2 ScreenDimensions;
};

class DeferredLightTechnique
{
public:
    DeferredLightTechnique();

    void Initialize();
    void Destroy();
    void Render();

protected:
    Engine* engInst = nullptr;
    RenderTarget* tarDepthStencil = nullptr;
    RenderTarget* tarLightAccumulation = nullptr;
    RenderTarget* tarDiffuse = nullptr;
    RenderTarget* tarSpecular = nullptr;
    RenderTarget* tarNormal = nullptr;
    Array<ID3D11RenderTargetView*> GBuffer;
    Array<ID3D11ShaderResourceView*> GBufferSRV;
    Array<D3D11_VIEWPORT> GBufferViewports;

    Buffer<LightIndexBuffer>* lightIndexBuf = nullptr;
    Buffer<ScreenToWorldParams>* screenToWorldBuf = nullptr;
    
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    ID3D11BlendState* blendState = nullptr;
    
    Array<PostProcessVertex> vertices;
    Array<int32_t> indexes;
    
    Shader* deferredLightShader = nullptr;
    void CreateConstantBuffers();
    void CreateShader();
    void CreateVertexes();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateBlendState();
    void CreateRasterizerState();
    void GBufferPass();
    void LightingPass();
    void PreRenderLightPassByLightID(int32_t lightId);
};
