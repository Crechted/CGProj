#pragma once
#include <string>

#include "assimp/scene.h"

class Mesh;

class MeshImporter
{
public:
    static Mesh* ImportMesh(const std::string& pathModel, const wchar_t* pathTex = nullptr, bool centering = true, uint32_t numMesh = 0);
    static bool ImportMesh(const std::string& pathModel, Mesh* resMesh, bool centering = true, uint32_t numMesh = 0);

private:
    static void FillMeshData(const aiScene* sce, Mesh* resMesh, bool centering = true, uint32_t numMesh = 0);
};
