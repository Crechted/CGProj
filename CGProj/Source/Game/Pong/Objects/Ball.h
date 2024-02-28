#pragma once
#include "PongObject.h"
#include "../../GameSquare.h"

class Ball : public PongObject
{
public:
    Ball(Vector3 center = {0.0, 0.0, 0.5},
        Vector3 extends = {0.02, 0.02, 0.2},
        Vector3 color = {1.0, 1.0, 1.0});

    void CreateComponents() override;
    void OnBeginOverlapped(Object* other) override;

    void Update(float timeTick) override;
};
