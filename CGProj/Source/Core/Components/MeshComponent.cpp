#include "MeshComponent.h"
#include "TextureComponent.h"
#include "TriangleComponent.h"
#include "Core/Engine.h"
#include "Core/Objects/Mesh.h"

MeshComponent::MeshComponent(const std::string& pathModel, const wchar_t* pathTex)
{
    engInst = engInst ? engInst : &Engine::GetInstance();
    mesh = new Mesh(pathModel, pathTex);
    mesh->GetSceneComponent()->AttachTo(this);
}

MeshComponent::MeshComponent()
{
    engInst = engInst ? engInst : &Engine::GetInstance();
    mesh = new Mesh();
    mesh->GetSceneComponent()->AttachTo(this);
}

void MeshComponent::Initialize()
{
    SceneComponent::Initialize();    
    mesh->Initialize();
}

void MeshComponent::PreDraw()
{
    SceneComponent::PreDraw();
    mesh->PreDraw();
}

void MeshComponent::Draw()
{
    SceneComponent::Draw();
    mesh->Draw();
}

void MeshComponent::Update(float timeTick)
{
    SceneComponent::Update(timeTick);
    //AddLocation(Vector3(1.0f)*timeTick);
    mesh->Update(timeTick);
}

void MeshComponent::DestroyResource()
{
    SceneComponent::DestroyResource();
    if (mesh) mesh->Destroy();
}

CollisionComponent* MeshComponent::GetMeshCollision() const
{
    return mesh->GetCollision();
}
