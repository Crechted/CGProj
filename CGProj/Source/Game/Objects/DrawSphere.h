#pragma once
#include "SimpleMath.h"
#include "Core/Objects/Object.h"

using namespace DirectX::SimpleMath;
class DrawComponent;

class DrawSphere : public Object
{
public:
    DrawSphere();
    void Initialize() override;    
    
    float radius = 0.5f;
    int32_t sliceCount = 16;
    int32_t stackCount = 16;
    Vector3 color = Vector3(1.0f);

    bool doDifColor = false;
    bool drawFirstHalf = true;
    bool drawSecondHalf = true;
    bool drawUp = true;
    bool drawDown = true;
    
    DrawComponent* triangleComp;
};
