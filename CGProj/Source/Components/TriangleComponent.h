#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "Inc/SimpleMath.h"


class Movement2DComponent;

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

    void SetPoints(DirectX::XMFLOAT4* pts, int32_t count)
    {
        points = pts;
        countPoints = count;
    };

    void SetIndexes(int32_t* ids, int32_t count)
    {
        indexes = ids;
        idxCount = count;
    };
    
    
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3DBlob* vertexBC;
    ID3DBlob* pixelBC;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    UINT idxCount = 6;

    UINT numElements = 2;

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

private:
    bool CompileVertexBC();
    bool CompilePixelBC();
    bool CheckResCompile(ID3DBlob* errorVertexCode, const HRESULT& res, LPCWSTR pFileName);
    void CreateShaders();
    void CreateLayout();

    void CreateVertexBuffer();
    void CreateIndexBuffer();

    void CreateAndSetRasterizerState();

    D3D11_BUFFER_DESC vertexBufDesc;
    D3D11_BUFFER_DESC indexBufDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;

    ID3D11Buffer* vb;
    ID3D11Buffer* ib;

    DirectX::XMFLOAT4* points;
    int32_t countPoints;

    int32_t* indexes;

    
    UINT* strides;
    UINT* offsets;
    float totalTime = 0;
};
