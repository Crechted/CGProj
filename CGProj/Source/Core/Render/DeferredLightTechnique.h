#pragma once
#include "Utils/Array/Array.h"
#include <d3d11.h>
#include "SimpleMath.h"
#include "Core/CoreTypes.h"
#include "Game/Components/LightComponents/LightComponent.h"

class BlendState;
class Shader;
class Buffer;
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
    Matrix ViewProj;
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

    Buffer* lightIndexBuf = nullptr;
    Buffer* screenToWorldBuf = nullptr;
    
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    BlendState* blendState = nullptr;

    ID3D11RasterizerState* rastStateCullFront;
    ID3D11RasterizerState* rastStateCullBack;
    
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    
    Shader* quadShader = nullptr;
    Shader* fullQuadShader = nullptr;
    Shader* volumeShader = nullptr;
    Shader* allQuadShader = nullptr;
    Shader* allVolumeShader = nullptr;
    Shader* renderOneLightTypeShader = nullptr;

    ID3D11DepthStencilState* DSSGreater = nullptr;
    ID3D11DepthStencilState* DSSLess = nullptr;
private:    
    Array<ID3D11RenderTargetView*> GBuffer;
    Array<ID3D11ShaderResourceView*> GBufferSRV;
    Array<D3D11_VIEWPORT> GBufferViewports;

    Array<LightComponent*> directionalLights;
    Array<LightComponent*> spotLights;
    Array<LightComponent*> pointLights;
    Array<LightData> curLightsData;

    
    void CreateConstantBuffers();
    void CreateShaders();
    void CreateVertices();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateBlendState();
    void CreateDepthStencilStates();
    void CreateRasterizerState();
    void GBufferPass();
    void DrawLightVolume(LightComponent* light, Shader* curShader);
    void DrawDirectionalLightVolume(Shader* curShader);
    void DrawLightsByType(LightType type);
    void BindScreenToWorldData();
    void LightingPass();
    void SortLights();
    void PreRenderLightPassByLightID(int32_t lightId);
    void PreRenderLightPassByLights(const Array<LightComponent*>& lights);
};
