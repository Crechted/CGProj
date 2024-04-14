#pragma once
#include <DirectXCollision.h>

#include "Core/Objects/Object.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
class DrawComponent;
class Movement2DComponent;
class GameComponent;

class GameSquare : public Object
{
public:
    GameSquare(Vector3 center = {0.f, 0.f, 0.f},
        Vector3 extends = {1.f, 1.f, 1.f},
        Vector3 color = {1.f, 1.f, 1.f});
    ~GameSquare() { GameSquare::Destroy(); };

    virtual void CreateComponents();
    void Initialize() override;
    virtual void InitPoints();
    void Draw() override;
    void Update(float timeTick) override;
    virtual void UpdateCenter();
    void Destroy() override;

    DirectX::BoundingBox BoxCollision;
    Movement2DComponent* moveComp = nullptr;
    DrawComponent* boxSprite = nullptr;

    Vector3 center;
    Vector3 extends;
    Vector3 color;    
    Vector3 curCenter;

    virtual void OnBeginOverlapped(Object* other);
    virtual void OnEndOverlapped(Object* other);
};
