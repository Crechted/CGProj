#pragma once

#include <d3d11.h>

#include "Core/CoreTypes.h"
#include "Core/Components/GameComponent.h"
#include "Game/Camera.h"

class TriangleComponent;
class SceneComponent;

struct LightData
{
    Matrix mViewProj;

    Vector4 posWS;
    Vector4 directionWS;
    Vector4 posVS;
    Vector4 directionVS;
    Vector4 color;

    float spotlightAngle = 0.0f;
    float range = 0.0f;
    alignas(4) bool enabled = true;
    uint32_t type = 0;
};

class LightComponent : public GameComponent
{
public:
    LightComponent();
    void Initialize() override;
    void DestroyResource() override;
    ID3D11ShaderResourceView* GetOutputTexture() const { return (&outputTextureSRV)[currentCascadeID]; }
    D3D11_VIEWPORT* GetViewport() { return &outputViewPort; }
    virtual void UpdateSubresource();
    virtual void UpdateShaderResources();
    SceneComponent* GetSceneComponent() const { return sceneComponent; }
    void SetDepthStencil();
    void ClearDepthStencil();
    void Update(float timeTick) override;

    virtual void SetCurrentCascadeData(uint32_t idx) { currentCascadeID = idx; }

    EyeViewData GetEyeData() const { return eyeData; }
    LightData GetLightData() const { return lightData; }
    virtual void SetLightData(const LightData& data) { lightData = data; }
    void SetColor(Vector4 col) { lightData.color = col; }

    virtual void GetLightVolume(Array<VertexNoTex>& vertices, Array<int32_t>& indexes);
    virtual void GetLightVolumeBuffers(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);

protected:
    LightData lightData;
    ID3D11Buffer* lightBuffer = nullptr;
    Array<VertexNoTex> volumeVertices;
    Array<int32_t> volumeIndexes;
    ID3D11Buffer* vertexBuf = nullptr;
    ID3D11Buffer* indexBuf = nullptr;

    SceneComponent* sceneComponent;
    ID3D11ShaderResourceView* outputTextureSRV = nullptr; // Shader resource view corresponding to the output texture (or shadow map)
    ID3D11DepthStencilView* outputTextureDSV = nullptr;   // Depth/template view (or shadow map) used for output texture
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_VIEWPORT outputViewPort = {}; // Viewport used for output


    ID3D11SamplerState* sampShadow = nullptr;

    EyeViewData eyeData;

    bool m_GenerateMips = false; // Whether to generate mipmap chain
    uint32_t texWidth = 2048;
    uint32_t texHeight = 2048;

    const wchar_t* shadowMapName = L"T_ShadowMap";

    uint32_t currentCascadeID = 0;

private:
    void CreateShadowMappingData();
    void CreateLightBuffer();
    void CreateVertices();
    void CreateVertexBuffer();
    void CreateIndexBuffer();

    void AddShadowMap();
    void RemoveShadowMap();
};

class CascadeShaderManager
{
public:
    static Array<Vector4> GetFrustumCorners(const Matrix& view, const Matrix& proj);
    static Vector3 GetFrustumCenter(const Array<Vector4>& corners);
    static Matrix GetOrthographicProjByCorners(const Array<Vector4>& corners, const Matrix& LightView);
};
