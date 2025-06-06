﻿#pragma once
#include <string>
#include "SceneComponent.h"

class CollisionComponent;
class Mesh;

class MeshComponent : public SceneComponent
{
public:
    MeshComponent(const std::string& pathModel, const wchar_t* pathTex = nullptr);
    MeshComponent();

    void Initialize() override;
    void Update(float timeTick) override;

    void PreDraw() override;
    void Draw() override;
    void DestroyResource() override;

    Mesh* GetMesh() const {return mesh;}
    CollisionComponent* GetMeshCollision() const;
    
protected:
    Mesh* mesh;
};
