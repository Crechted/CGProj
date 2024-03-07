#pragma once

#include "GameComponent.h"
#include "../Core/Input/Keys.h"
#include "Inc/SimpleMath.h"

class SceneComponent;
struct MouseMoveEventArgs;
using namespace DirectX::SimpleMath;

class MovementComponent : public GameComponent
{
public:
    bool SearchSceneComponent();
    void Initialize() override;
    void Reload() override;
    void Draw() override {}
    void Update(float timeTick) override;

    SceneComponent* sceneComp;

    Keys inputForward = Keys::W;
    Keys inputBackward = Keys::S;
    Keys inputLeft = Keys::A;
    Keys inputRight = Keys::D;
    Keys inputUp = Keys::Space;
    Keys inputDown = Keys::C;

    float speed = 0.5;
    float sensitive = 0.5;
    
    Vector3 delLocation;
    Vector3 delRotation;

protected:
    virtual void HandleInputByKey();
    virtual void CalcOffset(float timeTick);
    virtual void OnKeyDown(Keys key);
    virtual void OnKeyUp(Keys key);
    virtual void OnMouseMove(const MouseMoveEventArgs& mouseEvent);

};
