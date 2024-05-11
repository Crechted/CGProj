#pragma once

#include <d3d11.h>
#include "Core/Components/GameComponent.h"
#include "Game/Camera.h"

class SceneComponent;

class LightComponent : public GameComponent
{
public:
    LightComponent();
    void Initialize() override;
    void DestroyResource() override;
    ID3D11ShaderResourceView* GetOutputTexture() const { return (&outputTextureSRV)[currentCascadeID]; }
    D3D11_VIEWPORT* GetViewport() { return &outputViewPort; }

    SceneComponent* GetSceneComponent() const { return sceneComponent; }
    EyeViewData GetEyeData() const { return eyeData; }
    void SetDepthStencil();
    void ClearDepthStencil();

    virtual void SetCurrentCascadeData(uint32_t idx) { currentCascadeID = idx; }

protected:
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
