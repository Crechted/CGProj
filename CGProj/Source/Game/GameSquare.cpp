#include "GameSquare.h"

#include "../Components/Movement2DComponent.h"
#include "../Components/TriangleComponent.h"
#include "../Core/Game.h"


using namespace DirectX::SimpleMath;

GameSquare::GameSquare(Vector3 center, Vector3 extends, Vector3 color)
    : center(center), extends(extends), color(color)
{
}

void GameSquare::CreateComponents()
{
    moveComp = CreateComponent<Movement2DComponent>();
    boxSprite = CreateComponent<TriangleComponent>();
}

void GameSquare::Initialize()
{
    beginOverlapped.AddRaw(this, &GameSquare::OnBeginOverlapped);
    endOverlapped.AddRaw(this, &GameSquare::OnEndOverlapped);
    CreateComponents();
    InitPoints();
    if (boxSprite) boxSprite->Initialize();
    if (moveComp) moveComp->Initialize();

    BoxCollision.Center = center;
    BoxCollision.Extents = extends;
}

void GameSquare::InitPoints()
{
    boxSprite->SetPoints(new DirectX::XMFLOAT4[8]
    {
        DirectX::XMFLOAT4(center.x + extends.x, center.y + extends.y, 0.5f, 1.0f),
        DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(center.x - extends.x, center.y - extends.y, 0.5, 1.0f),
        DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(center.x + extends.x, center.y - extends.y, 0.5f, 1.0f),
        DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
        DirectX::XMFLOAT4(center.x - extends.x, center.y + extends.y, 0.5f, 1.0f),
        DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f),
    }, 8);
    boxSprite->SetIndexes(new int32_t[6]{0, 1, 2, 1, 0, 3}, 6);
}

void GameSquare::Draw()
{
    moveComp->Draw();
    boxSprite->Draw();
}

void GameSquare::Update(float timeTick)
{
    Object::Update(timeTick);
    UpdateCenter();
}

void GameSquare::UpdateCenter()
{
    curCenter.x = center.x + moveComp->GetOffset().x;
    curCenter.y = center.y + moveComp->GetOffset().y;
    BoxCollision.Center = curCenter;
}


void GameSquare::Destroy()
{
    delete moveComp;
    delete boxSprite;
}

void GameSquare::OnBeginOverlapped(Object* other)
{
}

void GameSquare::OnEndOverlapped(Object* other)
{
}
