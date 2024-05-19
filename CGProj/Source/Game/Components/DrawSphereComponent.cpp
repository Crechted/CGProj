#include "DrawSphereComponent.h"
#include "Core/Engine.h"
#include "Game/Objects/Drawable/DrawSphere.h"

DrawSphereComponent::DrawSphereComponent()
{
    engInst = engInst ? engInst : &Engine::GetInstance();
    sphere = new DrawSphere();
}

void DrawSphereComponent::Initialize()
{
    if (!sphere) return;
    sphere->radius = radius;
    sphere->sliceCount = sliceCount;
    sphere->stackCount = stackCount;
    sphere->color = color;
    sphere->doDifColor = doDifColor;
    sphere->drawFirstHalf = drawFirstHalf;
    sphere->drawSecondHalf = drawSecondHalf;
    sphere->drawUp = drawUp;
    sphere->drawDown = drawDown;
    sphere->Initialize();
    SceneComponent::Initialize();
}

void DrawSphereComponent::Update(float timeTick)
{
    if (!sphere) return;
    SceneComponent::Update(timeTick);
}

void DrawSphereComponent::Draw()
{
    if (!sphere) return;
    SceneComponent::Draw();
    sphere->Draw();
}

void DrawSphereComponent::DestroyResource()
{
    SceneComponent::DestroyResource();
    if (!sphere) sphere->Destroy();
}
