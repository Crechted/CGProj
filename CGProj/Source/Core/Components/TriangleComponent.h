#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "SimpleMath.h"
#include "Utils/Array/Array.h"


enum class RenderState;
class Shader;
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
    DirectX::XMFLOAT2 tex;
};

class TriangleComponent : public GameComponent
{
public:
    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;
    void Reload() override;
    void UpdateData();
    void Update(float timeTick) override;

    int32_t GetNumVertices() const { return vertices.size(); }
    void AddVertex(const Vertex& vertex);
    void AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& norm, const DirectX::XMFLOAT2& tex);
    void SetVertices(const Array<Vertex>& verts);
    void SetVertices(const Array<D3DVertex>& pts);

    void SetShader(Shader* difShader);
    void SetDefaultShader();

    int32_t GetNumIndexes() const { return indexes.size(); }
    void AddIndex(int32_t idx);
    void SetIndexes(const Array<int32_t>& idxs) { indexes = idxs; }
    void SetIndexes(int32_t* idxs, int32_t count);

    D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    D3D11_CULL_MODE cullMode = D3D11_CULL_FRONT;
    D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID;
    bool isAntialiasedLine = false;

protected:
    void CreateDefaultShader();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateAndSetRasterizerState();

    TriangleDrawData* curDrawData;
    Array<D3DVertex> vertices;

    Array<int32_t> indexes;
    Array<TriangleDrawData*> drawsData;

    float totalTime = 0;

    LPCWSTR pFileName = L"./Resource/Shaders/TriangleShader.hlsl";
    Shader* curShader = nullptr;
    Shader* defShader = nullptr;

private:
    ID3D11RasterizerState* rastState = nullptr;
    ID3D11RasterizerState* rastStateShadow = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;

    Shader* cascadeShader = nullptr;
    Shader* shadowMappingShader = nullptr;
    
    void OnChangeRenderState(RenderState state);
    void CreateCascadeShader();
    void CreateShadowMappingShader();
};
