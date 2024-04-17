#include "BoxComponent.h"

#include "Core/Engine.h"
#include "Core/Components/Collisions/BoxCollision.h"
#include "Game/Objects/Box.h"

BoxComponent::BoxComponent(const wchar_t* pathTex, Vector3 position, Vector3 size, Vector4 color)
{
    if (mesh) mesh->Destroy();
    mesh = new Box(pathTex, Vector3::Zero, size, color);
    
    mesh->SetCollision(mesh->CreateComponent<BoxCollision>(size));    
    mesh->GetSceneComponent()->AttachTo(this);
    //mesh->GetCollision()->initPosition = position;
    initPosition = position;
}
