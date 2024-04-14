#pragma once
#include "SimpleMath.h"
#include "Core/Components/GameComponent.h"
#include "Core/Input/InputDevice.h"
#include "Core/Input/Keys.h"

using namespace DirectX::SimpleMath;

class SceneComponent;

class PlayerMovement : public GameComponent
{
public:
    bool SearchSceneComponent();
    
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
    SceneComponent* movableComp = nullptr;
    virtual void HandleInputByKey();
    virtual void CalcOffset(float timeTick);
    virtual void OnKeyDown(Keys key);
    virtual void OnKeyUp(Keys key);
    virtual void OnMouseMove(const MouseMoveEventArgs& mouseEvent);
};
