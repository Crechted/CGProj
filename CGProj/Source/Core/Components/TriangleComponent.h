#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "SimpleMath.h"
#include "Utils/Array/Array.h"


struct Vertex;
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

struct D3DVertex
{
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 norm;
    DirectX::XMFLOAT4 worldLoc;
    DirectX::XMFLOAT2 tex;
};

class TriangleComponent : public GameComponent
{
public:
    ~TriangleComponent();

    void Initialize() override;
    void DestroyResource() override;
    void PreDraw() override;
    void Draw() override;
    void Reload() override;
    void UpdateData();
    void Update(float timeTick) override;

    int32_t GetNumVertices() const { return vertices.size(); }
    void AddVertex(const Vertex& vertex);
    void AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& norm, const DirectX::XMFLOAT2& tex);
    void SetVertices(const Array<Vertex>& verts);
    void SetVertices(const Array<D3DVertex>& pts);


    int32_t GetNumIndexes() const { return indexes.size(); }
    void AddIndex(int32_t idx);
    void SetIndexes(const Array<int32_t>& idxs) { indexes = idxs; }
    void SetIndexes(int32_t* idxs, int32_t count);

    //UINT idxCount = 6;
    UINT numElements = 4;

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    D3D11_CULL_MODE cullMode = D3D11_CULL_NONE;
    D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID;
    bool isAntialiasedLine = false;
    void SetWorldLoc(DirectX::SimpleMath::Vector3 loc) { worldLoc = loc; }

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
    Array<D3DVertex> vertices;
    //int32_t countPoints;

    Array<int32_t> indexes;

    Array<TriangleDrawData*> drawsData;

    UINT* strides;
    UINT* offsets;
    float totalTime = 0;

    LPCWSTR pFileName = L"./Resource/Shaders/TriangleShader.hlsl";
    LPSTR VSname = (LPSTR)"VSMain";
    LPSTR PSname = (LPSTR)"PSMain";

    DirectX::SimpleMath::Vector3 worldLoc;

private:
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};
