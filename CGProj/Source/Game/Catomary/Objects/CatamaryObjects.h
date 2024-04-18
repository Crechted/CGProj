#pragma once
#include "Core/Objects/Mesh.h"

class CatamaryObjects : public Mesh
{
public:
    CatamaryObjects(const std::string& pathModel, const wchar_t* pathTex = nullptr);

    void Initialize() override;

    void OnBeginOverlap(CollisionComponent* other);
};
