#pragma once
#include "Core/Objects/Object.h"
#include <DirectXCollision.h>
#include <string>

#include "DDSTextureLoader.h"
#include "Core/CoreTypes.h"


class DrawComponent;
class TriangleComponent;
class TextureComponent;
class SceneComponent;

class Mesh : public Object
{
public:
    Mesh(const std::string& pathModel , const wchar_t* pathTex = nullptr);
    Mesh();
    void InitDebugCollision();
    void UpdateCollision();
    void Initialize() override;
    void Update(float timeTick) override;
    void Draw() override;
    
    DirectX::BoundingBox* GetCollision();
    TextureComponent* GetTextureComponent() const {return textureComp;}
    SceneComponent* GetSceneComponent() const {return sceneComp;}

    int32_t GetNumVertices() const;
    void SetAABB(DirectX::SimpleMath::Vector3 min, DirectX::SimpleMath::Vector3 max);
    
    void SetVertices(const Array<Vertex>& vertices) const;
    void AddVertex(const Vertex& vertex) const;

    void SetIndices(const Array<int32_t>& indices) const;
    void AddIndex(int32_t index) const;

    void SetTexture(ID3D11ShaderResourceView* tex);
    void SetTexture(const wchar_t* path);

    void SetCollisionVisibility(bool visible) {showCollision = visible;}
    
    MulticastDelegate<Object*> beginOverlapped; 
    MulticastDelegate<Object*> endOverlapped;
    
protected:
    SceneComponent* sceneComp;
    TriangleComponent* triangleComp;    
    TextureComponent* textureComp;

    DrawComponent* debugCollision;
    
    const std::string initPathModel;
    const wchar_t* initPathTex = nullptr;

    DirectX::SimpleMath::Vector3 maxAABB;
    DirectX::SimpleMath::Vector3 minAABB;
    bool showCollision = false;
    DirectX::BoundingBox boxCollision;

    void OnBeginOverlap(Object* other);
};
