#include "Mesh.h"

#include "Core/Components/DrawComponent.h"
#include "Utils/MeshImporter.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Components/TextureComponent.h"
#include "Core/Components/TriangleComponent.h"
#include "Game/Objects/Box.h"

Mesh::Mesh(const std::string& pathModel, const wchar_t* pathTex)
    : initPathModel(pathModel), initPathTex(pathTex)
{
    sceneComp = CreateComponent<SceneComponent>();
    triangleComp = CreateComponent<TriangleComponent>();
    textureComp = CreateComponent<TextureComponent>();
    debugCollision = CreateComponent<DrawComponent>();
}

Mesh::Mesh()
{
    sceneComp = CreateComponent<SceneComponent>();
    triangleComp = CreateComponent<TriangleComponent>();
    textureComp = CreateComponent<TextureComponent>();
    debugCollision = CreateComponent<DrawComponent>();
}

void Mesh::Initialize()
{
    if (!initPathModel.empty()) MeshImporter::ImportMesh(initPathModel, this);
    SetTexture(initPathTex);
    InitDebugCollision();
    beginOverlapped.AddRaw(this, &Mesh::OnBeginOverlap);
    Object::Initialize();
}

void Mesh::InitDebugCollision()
{
    Array<VertexNoTex> vertices;
    Array<int32_t> indexes;
    const Vector3 size = maxAABB - minAABB;
    
    boxCollision.Extents = DirectX::XMFLOAT3(size/2);
    boxCollision.Center = DirectX::XMFLOAT3(minAABB + size/2);
    Box::CreateDrawBoxByTopology(minAABB + size/2, size, Vector4(0.5f, 0.0f, 1.0f, 1.0f), vertices, indexes, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    debugCollision->topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    debugCollision->SetVertices(vertices);
    debugCollision->SetIndexes(indexes);
}

void Mesh::Update(float timeTick)
{
    Object::Update(timeTick);
    UpdateCollision();
}

void Mesh::UpdateCollision()
{
    const DirectX::FXMMATRIX mat = sceneComp->GetWorldMatrix();
    boxCollision.Transform(boxCollision, mat);
}

void Mesh::Draw()
{
    for (auto gameComp : gameComponents)
    {
        if (gameComp == debugCollision && !showCollision) continue;
        gameComp->Draw();
    }
}

DirectX::BoundingBox* Mesh::GetCollision()
{
    return &boxCollision;
}

int32_t Mesh::GetNumVertices() const
{
    return triangleComp->GetNumVertices();
}

void Mesh::SetAABB(Vector3 min, Vector3 max)
{
    minAABB = min;
    maxAABB = max;
}

void Mesh::SetVertices(const Array<Vertex>& vertices) const
{
    for (const auto& vert : vertices)
    {
        triangleComp->AddVertex(vert);
    }
}

void Mesh::AddVertex(const Vertex& vertex) const
{
    triangleComp->AddVertex(vertex);
}

void Mesh::SetIndices(const Array<int32_t>& indices) const
{
    triangleComp->SetIndexes(indices);
}

void Mesh::AddIndex(int32_t index) const
{
    triangleComp->AddIndex(index);
}

void Mesh::SetTexture(ID3D11ShaderResourceView* tex)
{
    if (tex)
    {
        if (textureComp) textureComp->DestroyResource();
        textureComp->SetTexture(tex);
    }
}

void Mesh::SetTexture(const wchar_t* path)
{
    if (path)
    {
        //if (texture) texture->DestroyResource();
        textureComp->SetTexture(path);
    }
}

void Mesh::OnBeginOverlap(Object* other)
{
    printf("Overlaped!");
}
