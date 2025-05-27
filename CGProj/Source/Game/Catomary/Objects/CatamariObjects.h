#pragma once
#include "Core/Objects/Mesh.h"

class CatamariObjects : public Mesh
{
public:
    CatamariObjects(const std::string& pathModel, const wchar_t* pathTex = nullptr);

    void Initialize() override;

    void OnBeginOverlap(CollisionComponent* other);
};
