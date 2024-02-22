#pragma once
#include <d3d11.h>
#include <directxmath.h>

#include "GameComponent.h"


class TriangleComponent : public GameComponent
{
public:
    TriangleComponent();
    ~TriangleComponent();

    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;
    void Reload() override;
    void Update(float timeTick) override;

    void SetPoints(DirectX::XMFLOAT4* pts, int32_t count) {points = pts; countPoints = count;};
    
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3DBlob* vertexBC;
    ID3DBlob* pixelBC;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    D3D11_BUFFER_DESC vertexBufDesc;
    D3D11_SUBRESOURCE_DATA vertexData;

    
private:    
    ID3D11RenderTargetView* rtv;
    ID3D11Buffer* vb;
    ID3D11Buffer* ib;
    
    DirectX::XMFLOAT4* points;
    int32_t countPoints;
    
    UINT* strides;
    UINT* offsets;
    float totalTime = 0;
};
