#pragma once
#include "Core/Components/SceneComponent.h"

class Camera;
enum class Keys;

class SpringArmComponent : public SceneComponent
{
public:
    SpringArmComponent(Camera* cam = nullptr);
    void Initialize() override;
    void Detach();
    void Reload() override;

    void SetControlCamera(Camera* cam);
    Camera* GetCamera() const { return controlCamera; }
    
    void Update(float timeTick) override;
    void OnKeyDown(Keys key);
    void OnKeyUp(Keys key);
    float springLenght = 0.0f;

protected:
    Camera* controlCamera;
    float delSpringLenght = 0.0f;
};
