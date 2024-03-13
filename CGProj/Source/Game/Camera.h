#pragma once
#include "../Components/MovementComponent.h"
#include "../Core/Object.h"
#include "Inc/SimpleMath.h"

class SpringArmComponent;
class SceneComponent;
using namespace DirectX::SimpleMath;

enum class ViewType : uint8_t
{
    General,
    Perspective,
    OrtFree,
    OrtXOZ,
    OrtYOZ,
    OrtXOY,    
};

class Camera : public Object
{
public:
    Camera(Vector3 initPosition = Vector3(2.0f, 2.0f, 2.0f), Vector3 initRotation = Vector3(-30.0f, 45.0f, 0.0f));
    void Reload() override;
    void Draw() override;
    void UpdateProjMatrix();
    void UpdateViewMatrix();
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
    float farPlane = 100.0f;

    float initScale = 1.0f;
    float speedScale = 1.0f;
    bool isPerspective = true;
    SceneComponent* sceneComp;
    MovementComponent* movementComp;

    SpringArmComponent* springArmComp;
    void OnKeyDown(Keys key);
    void OnKeyUp(Keys key);
    void OnMouse(const MouseMoveEventArgs& mouseData);

    ViewType viewType = ViewType::Perspective;
    Vector3 initPosition;
    Vector3 initRotation;
protected:
    float scale = 1.0f;
    float delScale;
};
