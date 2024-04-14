#include "MeshImporter.h"

#include <complex>
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "assimp/BlobIOSystem.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Core/Objects/Mesh.h"



Mesh* MeshImporter::ImportMesh(const std::string& pathModel, const wchar_t* pathTex)
{
    Assimp::Importer importer;
    const aiScene* sce = importer.ReadFile(pathModel, aiProcess_Triangulate | aiProcess_GenBoundingBoxes);
    if (!sce) return nullptr;
    printf("Meshes %d\n", sce->mNumMeshes);
    printf("Meshes %d\n", sce->mRootNode->mNumMeshes);
    Mesh* resMesh = Engine::GetInstance().CreateObject<Mesh>();
    resMesh->SetTexture(pathTex);
    FillMeshData(sce, resMesh);

    return resMesh;
}

bool MeshImporter::ImportMesh(const std::string& pathModel, Mesh* resMesh)
{
    Assimp::Importer importer;
    const aiScene* sce = importer.ReadFile(pathModel, aiProcess_Triangulate | aiProcess_GenBoundingBoxes);
    if (!sce) return false;
    FillMeshData(sce, resMesh);
    return true;
}

void MeshImporter::FillMeshData(const aiScene* sce, Mesh* resMesh)
{
    if(!resMesh) return;
    
    int32_t numVertices = 0;
    for (int32_t i = 0; i < sce->mNumMeshes; i++)
    {
        const auto mesh = sce->mMeshes[i];
        for (int32_t j = 0; j < mesh->mNumVertices; j++)
            numVertices++;
    }
    int32_t sideSize = (int32_t)std::sqrt(numVertices) + 1;

    int32_t count = 0;
    Vector3 maxAABB = Vector3::Zero;
    Vector3 minAABB = Vector3::Zero;
    for (int32_t i = 0; i < sce->mNumMeshes; i++)
    {
        const auto mesh = sce->mMeshes[i];
        printf("Vertex %d\n", mesh->mNumVertices);
        Vector3::Max(maxAABB, Vector3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z), maxAABB);
        Vector3::Min(minAABB, Vector3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z), minAABB);
        for (int32_t j = 0; j < mesh->mNumVertices; j++)
        {
            Vertex newVert = {Vector4(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z, 1.0f),
                              Vector4(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z, 1.0f),
                              mesh->HasTextureCoords(0)
                                  ? Vector2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
                                  : Vector2((float)((count + j) % sideSize) / sideSize, (float)((count + j) / sideSize) / sideSize)
            };
            /*printf("x = %f, y = %f\n", newVert.texture.x,
                newVert.texture.y);*/
            //Vector4(color.r, color.g, color.b, color.a)};
            resMesh->AddVertex(newVert);
            resMesh->AddIndex(count + j);
        }

        count += mesh->mNumVertices;
    }
    resMesh->SetAABB(minAABB, maxAABB);
}
