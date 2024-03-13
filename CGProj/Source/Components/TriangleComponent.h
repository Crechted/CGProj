#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "../Utils/Array/Array.h"


class Movement2DComponent;

struct TriangleDrawData
{
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};

class TriangleComponent : public GameComponent
{
public:
    TriangleComponent();
    ~TriangleComponent();

    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;
    void Reload() override;
    void UpdateData();
    void Update(float timeTick) override;

    int32_t GetNumPoints() const {return points.size();}
    void AddPoint(DirectX::XMFLOAT4 point, DirectX::XMFLOAT4 color);
    void SetPoints(const Array<DirectX::XMFLOAT4>& pts) { points = pts; }
    void SetPoints(DirectX::XMFLOAT4* pts, int32_t count);

    
    int32_t GetNumIndexes() const {return indexes.size();}
    void AddIndex(int32_t idx);
    void SetIndexes(const Array<int32_t>& idxs) { indexes = idxs; }
    void SetIndexes(int32_t* idxs, int32_t count);

    //UINT idxCount = 6;
    UINT numElements = 2;

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    D3D11_CULL_MODE cullMode = D3D11_CULL_NONE;
    D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID;
    bool isAntialiasedLine = false;

protected:
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
    
    ID3DBlob* vertexBC;
    ID3DBlob* pixelBC;    
    

    TriangleDrawData* curDrawData;

    Array<DirectX::XMFLOAT4> points;
    //int32_t countPoints;

    Array<int32_t> indexes;
    
    Array<TriangleDrawData*> drawsData;

    UINT* strides;
    UINT* offsets;
    float totalTime = 0;

private:
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};
