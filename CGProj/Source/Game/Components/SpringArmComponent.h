#pragma once
#include "../../Components/SceneComponent.h"

class Camera;
enum class Keys;

class SpringArmComponent : public SceneComponent
{
public:
    SpringArmComponent();
    void Initialize() override;
    void Detach();
    void Reload() override;

    Camera* controlCamera;
    void Update(float timeTick) override;
    void OnKeyDown(Keys key);
    void OnKeyUp(Keys key);
    float springLenght = 0.0f;

protected:
    float delSpringLenght = 0.0f;    
};
