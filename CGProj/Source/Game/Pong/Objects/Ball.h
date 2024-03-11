#pragma once
#include "PongObject.h"
#include "../../GameSquare.h"

class Ball : public PongObject
{
public:
    Ball(Vector3 center = {0.0f, 0.0f, 0.5f},
        Vector3 extends = {0.02f, 0.02f, 0.2f},
        Vector3 color = {1.0f, 1.0f, 1.0f});

    void CreateComponents() override;
    void OnBeginOverlapped(Object* other) override;

    void Update(float timeTick) override;
};
