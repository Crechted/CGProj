#include "DrawBoxComponent.h"

#include "Game/Objects/Drawable/DrawBox.h"

DrawBoxComponent::DrawBoxComponent(Vector3 position, Vector3 size, Vector4 color, D3D_PRIMITIVE_TOPOLOGY topology)
{
    box = new DrawBox(position, size, color, topology);
    box->outer = this;
}

void DrawBoxComponent::Update(float timeTick)
{
    box->Update(timeTick);
    SceneComponent::Update(timeTick);
}

void DrawBoxComponent::Initialize()
{
    box->Initialize();
    SceneComponent::Initialize();
}

void DrawBoxComponent::Draw()
{
    SceneComponent::Draw();
    box->Draw();
}
void DrawBoxComponent::DestroyResource()
{
    box->Destroy();
    SceneComponent::DestroyResource();
}
