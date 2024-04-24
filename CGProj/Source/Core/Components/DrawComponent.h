#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "Utils/Array/Array.h"


struct TriangleDrawData;
struct VertexNoTex;
class Movement2DComponent;

struct D3DMinVertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 col;
};

class DrawComponent : public GameComponent
{
public:
    ~DrawComponent();

    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;
    void Reload() override;
    void UpdateData();
    void Update(float timeTick) override;

    int32_t GetNumVertices() const {return vertices.size();}
    void AddVertex(const VertexNoTex& vertex);
    void AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& col);
    void SetVertices(const Array<VertexNoTex>& verts);
    void SetVertices(const Array<DirectX::XMFLOAT4>& pts);
    void SetVertices(DirectX::XMFLOAT4* pts, int32_t count);

    
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

    //Array<DirectX::XMFLOAT4> vertices;
    Array<D3DMinVertex> vertices;

    Array<int32_t> indexes;
    
    Array<TriangleDrawData*> drawsData;

    UINT* strides;
    UINT* offsets;
    float totalTime = 0;
    
    LPCWSTR pFileName = L"./Resource/Shaders/DrawShader.hlsl";
    LPSTR VSname = (LPSTR)"VSMain";
    LPSTR PSname = (LPSTR)"PSMain";
private:
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};

