#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "GameComponent.h"
#include "Core/CoreTypes.h"

enum class RenderState;
class Shader;

class DrawComponent : public GameComponent
{
public:
    void Initialize() override;
    void DestroyResource() override;
    void Draw() override;
    void Reload() override;
    void UpdateData();
    void Update(float timeTick) override;

    int32_t GetNumVertices() const {return vertices.size();}
    void ClearVertex() {vertices.clear();};
    void AddVertex(const VertexNoTex& vertex);
    void AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& col);
    void SetVertices(const Array<VertexNoTex>& verts);
    void SetVertices(const Array<DirectX::XMFLOAT4>& pts);
    void SetVertices(DirectX::XMFLOAT4* pts, int32_t count);

    void SetShader(Shader* difShader);
    void SetDefaultShader();
    
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
    void CreateDefaultShader();
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateAndSetRasterizerState();

    /*D3D11_BUFFER_DESC vertexBufDesc;
    D3D11_BUFFER_DESC indexBufDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    D3D11_SUBRESOURCE_DATA indexData;*/

    Array<VertexNoTex> vertices;

    Array<int32_t> indexes;    

    float totalTime = 0;
    
    LPCWSTR pFileName = L"./Resource/Shaders/DrawShader.hlsl";

    Shader* curShader = nullptr;
    Shader* defShader = nullptr;
    
private:
    ID3D11RasterizerState* rastState;
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    
    Shader* cascadeShader = nullptr;
    Shader* shadowMappingShader = nullptr;
    
};

