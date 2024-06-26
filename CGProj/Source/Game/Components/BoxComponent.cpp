#include "BoxComponent.h"

#include "Core/Engine.h"
#include "Core/Components/Collisions/BoxCollision.h"
#include "Game/Objects/Drawable/Box.h"

BoxComponent::BoxComponent(const wchar_t* pathTex, Vector3 position, Vector3 size, Vector4 color, D3D_PRIMITIVE_TOPOLOGY topology)
{
    if (mesh) mesh->Destroy();
    mesh = new Box(pathTex, Vector3::Zero, size, color, topology);
    mesh->outer = this;
    mesh->SetCollision(mesh->CreateComponent<BoxCollision>(size));    
    mesh->GetSceneComponent()->AttachTo(this);
    //mesh->GetCollision()->initPosition = position;
    initPosition = position;
}
