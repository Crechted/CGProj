#pragma once
#include "PongObject.h"
#include "Game/GameSquare.h"

class Racket : public PongObject
{
public:
    Racket(Vector3 center = {0.9, 0.0, 0.5},
        Vector3 extends = {0.015, 0.15, 0.2},
        Vector3 color = {1.f, 1.f, 1.f});

    bool isAI = false;
    void CreateComponents() override;

    void OnBeginOverlapped(Object* other) override;
    void OnEndOverlapped(Object* other) override;
};
