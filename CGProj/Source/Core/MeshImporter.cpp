#include "MeshImporter.h"

#include "assimp/BlobIOSystem.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


MeshComponent* MeshImporter::ImportMesh(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* sce = importer.ReadFile(path, aiProcess_Triangulate);

    printf("Meshes %d\n", sce->mRootNode->mNumMeshes);
    const auto mesh = sce->mMeshes[sce->mRootNode->mMeshes[0]];
    printf("Vertex %d\n", mesh->mNumVertices);
    for (int32_t i = 0; i < mesh->mNumVertices; i++)
    {
        printf("x = %f, y = %f, z = %f\n", mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);
    }
    return nullptr;
}
