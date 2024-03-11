#pragma once
#include "../Components/MovementComponent.h"
#include "../Core/Object.h"
#include "Inc/SimpleMath.h"

class SceneComponent;
using namespace DirectX::SimpleMath;

class Camera : public Object
{
public:
    Camera(Vector3 initPosition = Vector3(2.0f, 2.0f, 2.0f), Vector3 initRotation = Vector3(0.0f, 0.0f, 0.0f));
    void Draw() override;
    void Initialize() override;
    void Update(float timeTick) override;

    Matrix mWorld;
    Matrix mView;
    Matrix mProj;
    Matrix mOrtho;

    float mAngle = 0.0f;
    float width = 0.01f;
    float height = 0.01f;
    float widthOrt = 2.0f;
    float heightOrt = 2.0f;
    float nearPlane = 0.01f;
    float farPlane = 1.0f;

    bool isPerspective = true;
    SceneComponent* sceneComponent;
    MovementComponent* movementComponent;
};
