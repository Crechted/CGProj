#pragma once
#include "Core/Objects/Object.h"
#include <string>

#include "DDSTextureLoader.h"
#include "Core/CoreTypes.h"


class CollisionComponent;
class DrawComponent;
class TriangleComponent;
class TextureComponent;
class SceneComponent;

class Mesh : public Object
{
public:
    Mesh(const std::string& pathModel, const wchar_t* pathTex = nullptr);
    Mesh();
    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;

    CollisionComponent* GetCollision() const;
    void SetCollision(CollisionComponent* newCollisionComp);
    TextureComponent* GetTextureComponent() const { return textureComp; }
    SceneComponent* GetSceneComponent() const { return sceneComp; }

    int32_t GetNumVertices() const;
    void SetAABB(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);
    void GetAABB(DirectX::SimpleMath::Vector3& min, DirectX::SimpleMath::Vector3& max) const
    {
        min = minAABB;
        max = maxAABB;
    }

    void SetVertices(const Array<Vertex>& vertices) const;
    void AddVertex(const Vertex& vertex) const;

    void SetIndices(const Array<int32_t>& indices) const;
    void AddIndex(int32_t index) const;

    void SetTexture(ID3D11ShaderResourceView* tex);
    void SetTexture(const wchar_t* path);

    void SetCollisionVisibility(bool visible);
    void SetCollisionEnabled(bool enable);
    bool CollisionEnabled() const;

protected:
    SceneComponent* sceneComp;
    TriangleComponent* triangleComp;
    TextureComponent* textureComp;
    CollisionComponent* collisionComp;

    std::string initPathModel;
    const wchar_t* initPathTex = nullptr;

    DirectX::SimpleMath::Vector3 maxAABB;
    DirectX::SimpleMath::Vector3 minAABB;

    void OnBeginOverlap(CollisionComponent* other);
};
