#pragma once
#include "Sphere.h"

class SkySphere : public Sphere
{
public:
    SkySphere(const wchar_t* pathTex = L"Resource/Textures/sky.jpeg",
        float radius = 500.0f,
        int32_t sliceCount = 64,
        int32_t stackCount = 64,
        bool drawFirstHalf = true,
        bool drawSecondHalf = true,
        bool drawUp = true,
        bool drawDown = true
        );
};
