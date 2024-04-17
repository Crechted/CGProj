#pragma once
#include "Core/Objects/Mesh.h"

class CatomaryObjects : public Mesh
{
public:
    CatomaryObjects(const std::string& pathModel, const wchar_t* pathTex = nullptr);

    void Initialize() override;

    void OnBeginOverlap(CollisionComponent* other);
};
