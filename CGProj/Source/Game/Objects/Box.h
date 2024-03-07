#pragma once
#include "../../Core/Object.h"

class BoxComponent;
class TriangleComponent;

class Box : public Object
{
public:
    Box();
    void Initialize() override;

    BoxComponent* boxComponent;
};
