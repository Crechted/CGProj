#pragma once
#include <string>

#include "assimp/scene.h"

class Mesh;

class MeshImporter
{
public:
    static Mesh* ImportMesh(const std::string& pathModel, const wchar_t* pathTex = nullptr);
    static bool ImportMesh(const std::string& pathModel, Mesh* resMesh);

private:
    static void FillMeshData(const aiScene* sce, Mesh* resMesh);
};
