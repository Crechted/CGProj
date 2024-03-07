#pragma once
#include "../Components/MovementComponent.h"
#include "../Core/Object.h"
#include "Inc/SimpleMath.h"

class SceneComponent;
using namespace DirectX::SimpleMath;

class Camera : public Object
{
public:
    Camera();
    void Initialize() override;
    void Update(float timeTick) override;

    Matrix mWorld;
    Matrix mView;
    Matrix mProj;
    Matrix mOrtho;

    float mAngle = 0.0f;        
    float width = 0.01;  
    float height = 0.01; 
    float widthOrt = 2.0;  
    float heightOrt = 2.0; 
    float nearPlane = 0.01f;      
    float farPlane = 1.0f;

    bool isPerspective = true;
    SceneComponent* sceneComponent;
    MovementComponent* movementComponent;
};
