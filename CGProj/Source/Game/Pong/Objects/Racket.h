#pragma once
#include "PongObject.h"
#include "Game/GameSquare.h"

class Racket : public PongObject
{
public:
    Racket(Vector3 center = {0.9f, 0.0f, 0.5f},
        Vector3 extends = {0.015f, 0.15f, 0.2f},
        Vector3 color = Vector3::One);

    bool isAI = false;
    void CreateComponents() override;

    void OnBeginOverlapped(Object* other) override;
    void OnEndOverlapped(Object* other) override;
};
