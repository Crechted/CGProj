#include "BoxComponent.h"

#include "../../Components/TriangleComponent.h"
#include "../../Core/Game.h"
#include "../Objects/Box.h"

BoxComponent::BoxComponent()
{
    game = game ? game : &Game::GetGame();
    box = new Box();
}

void BoxComponent::Initialize()
{
    if (!box) return;
    box->size = size;
    box->color = color;
    box->Initialize();
    SceneComponent::Initialize();
}

void BoxComponent::Update(float timeTick)
{
    if (!box) return;
    SceneComponent::Update(timeTick);
    box->Update(timeTick);
}

void BoxComponent::Draw()
{
    if (!box) return;
    SceneComponent::Draw();
    box->Draw();
}

void BoxComponent::DestroyResource()
{
    SceneComponent::DestroyResource();
    if (box) box->Destroy();
}
