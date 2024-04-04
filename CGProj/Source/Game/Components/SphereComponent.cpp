#include "SphereComponent.h"
#include "..\..\Core\Engine.h"
#include "../Objects/Sphere.h"

SphereComponent::SphereComponent()
{
    engInst = engInst ? engInst : &Engine::GetInstance();
    sphere = new Sphere();
}

void SphereComponent::Initialize()
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

void SphereComponent::Update(float timeTick)
{
    if (!sphere) return;
    SceneComponent::Update(timeTick);
}

void SphereComponent::Draw()
{
    if (!sphere) return;
    SceneComponent::Draw();
    sphere->Draw();
}

void SphereComponent::DestroyResource()
{
    SceneComponent::DestroyResource();
    if (!sphere) sphere->Destroy();
}
