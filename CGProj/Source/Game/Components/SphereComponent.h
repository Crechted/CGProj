#pragma once
#include "Core/Components/MeshComponent.h"

class SphereComponent: public MeshComponent
{
public:
    SphereComponent(const wchar_t* pathTex = nullptr,
        Vector3 position = Vector3::Zero,
        float radius = 0.5f,
        int32_t sliceCount = 16,
        int32_t stackCount = 16,
        bool drawFirstHalf = true,
        bool drawSecondHalf = true,
        bool drawUp = true,
        bool drawDown = true);
};
