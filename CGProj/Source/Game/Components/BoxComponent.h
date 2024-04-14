#pragma once
#include "Core/Components/MeshComponent.h"
#include "Core/Components/SceneComponent.h"
class Box;

class BoxComponent : public MeshComponent
{
public:
    BoxComponent(const wchar_t* pathTex = nullptr,
        Vector3 position = Vector3::Zero,
        Vector3 size = Vector3(0.2f),
        Vector4 color = Vector4(1.0f));
};
