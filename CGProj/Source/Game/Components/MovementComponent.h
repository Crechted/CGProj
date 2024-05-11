#pragma once

#include "Core/Components/GameComponent.h"
#include "SimpleMath.h"
#include "Core/Input/Keys.h"

struct MouseMoveEventArgs;
class SceneComponent;

class MovementComponent : public GameComponent
{
public:
    bool SearchSceneComponent();
    void Initialize() override;
    void Reload() override;

    void Draw() override
    {
    }

    void Update(float timeTick) override;

    SceneComponent* sceneComp;

    Keys inputForward = Keys::W;
    Keys inputBackward = Keys::S;
    Keys inputLeft = Keys::A;
    Keys inputRight = Keys::D;
    Keys inputUp = Keys::E;
    Keys inputDown = Keys::Q;

    float speed = 1.0;
    float sensitive = 10.0;

    DirectX::SimpleMath::Vector3 delLocation;
    DirectX::SimpleMath::Vector3 delRotation;

protected:
    SceneComponent* parentScene = nullptr;
    virtual void HandleInputByKey();
    virtual void CalcOffset(float timeTick);
    virtual void OnKeyDown(Keys key);
    virtual void OnKeyUp(Keys key);
    virtual void OnMouseMove(const MouseMoveEventArgs& mouseEvent);

};
