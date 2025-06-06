﻿#pragma once
#include "Core/Objects/Object.h"
#include "SimpleMath.h"


enum class Keys;
struct MouseMoveEventArgs;
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

struct EyeViewData
{
    Matrix GetViewProj() const {return mView*mProj;}
    Matrix mView = Matrix::Identity;
    Matrix mProj = Matrix::Identity;
    bool isCam = false;
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

    EyeViewData GetEyeData() const {return eyeData;}
    
    float mAngle = 0.0f;
    float width = 0.01f;
    float height = 0.01f;
    float widthOrt = 2.0f;
    float heightOrt = 2.0f;
    float nearPlane = 0.01f;
    float farPlane = 50.0f;

    float initScale = 1.0f;
    float speedScale = 1.0f;
    bool isPerspective = true;

    void OnKeyDown(Keys key);
    void OnKeyUp(Keys key);
    void OnMouse(const MouseMoveEventArgs& mouseData);

    void GetProjAndDistanceBySection(float nearLayer, float farLayer, Matrix& retProj, float& retDistanceToFar);
    SceneComponent* GetSceneComponent() const { return sceneComp; }
    void SetTargetView(SceneComponent* target) {targetView = target;}
    SceneComponent*  GetTargetView() {return targetView;}
    ViewType viewType = ViewType::Perspective;
    Vector3 initPosition;
    Vector3 initRotation;

protected:    
    EyeViewData eyeData;
    SceneComponent* sceneComp;

    SceneComponent* targetView = nullptr;
    float scale = 1.0f;
    float delScale;
};
