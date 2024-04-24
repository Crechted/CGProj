#include "Mesh.h"

#include "Core/Components/CollisionComponent.h"
#include "Core/Components/DrawComponent.h"
#include "Utils/MeshImporter.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Components/TextureComponent.h"
#include "Core/Components/TriangleComponent.h"
#include "Core/Components/Collisions/BoxCollision.h"

Mesh::Mesh(const std::string& pathModel, const wchar_t* pathTex, bool centering)
    : initPathModel(pathModel), initPathTex(pathTex), centering(centering)
{
    collisionComp = CreateComponent<BoxCollision>();
    sceneComp = CreateComponent<SceneComponent>();
    triangleComp = CreateComponent<TriangleComponent>();
    textureComp = CreateComponent<TextureComponent>();
    collisionComp->AttachTo(sceneComp);
}

Mesh::Mesh()
    : centering(true)
{
    collisionComp = CreateComponent<BoxCollision>();
    sceneComp = CreateComponent<SceneComponent>();
    triangleComp = CreateComponent<TriangleComponent>();
    textureComp = CreateComponent<TextureComponent>();
    collisionComp->AttachTo(sceneComp);
}

void Mesh::InitTexture()
{
    SetTexture(initPathTex);
}

void Mesh::InitMesh()
{
    if (!initPathModel.empty())
    {        
        MeshImporter::ImportMesh(initPathModel, this, centering);
    }
}

void Mesh::Initialize()
{
    InitMesh();
    InitTexture();
    Object::Initialize();
}

void Mesh::Update(float timeTick)
{
    Object::Update(timeTick);
}

void Mesh::Draw()
{
    Object::Draw();
}

CollisionComponent* Mesh::GetCollision() const
{
    return collisionComp && collisionComp->CollisionEnabled() ? collisionComp : nullptr;
}

void Mesh::SetCollision(CollisionComponent* newCollisionComp)
{
    if (!newCollisionComp) return;
    if (collisionComp) collisionComp->DestroyResource();
    gameComponents.remove(collisionComp);
    collisionComp = newCollisionComp;
    //gameComponents.insert(0, collisionComp);
    collisionComp->AttachTo(sceneComp);
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
    triangleComp->SetVertices(vertices);
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

void Mesh::SetCollisionVisibility(bool visible)
{
    if (collisionComp) collisionComp->SetCollisionVisibility(visible);
}

void Mesh::SetCollisionEnabled(bool enable)
{
    if (collisionComp) collisionComp->SetCollisionEnabled(enable);
}

bool Mesh::CollisionEnabled() const
{
    if (collisionComp) return collisionComp->CollisionEnabled();
    return false;
}
