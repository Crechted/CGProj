#pragma once
#include "SimpleMath.h"
#include "Core/Components/GameComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Input/Keys.h"

using namespace DirectX::SimpleMath;

class CatamariPawn;

class PlayerMoveComponent : public GameComponent
{
public:
    
    void Initialize() override;
    void Reload() override;

    void Draw() override
    {
    }

    void Update(float timeTick) override;

    Keys inputForward = Keys::W;
    Keys inputBackward = Keys::S;
    Keys inputLeft = Keys::A;
    Keys inputRight = Keys::D;

    float speed = 1.0;
    float sensitive = 10.0;

    Vector3 delLocation;
    Vector3 delRotation;

protected:
    CatamariPawn* pawn = nullptr;
    virtual void HandleInputByKey();
    virtual void CalcOffset(float timeTick);
    virtual void OnMouseMove(const MouseMoveEventArgs& mouseEvent);

private:    
    float rotSpeed = 140.0;
};
