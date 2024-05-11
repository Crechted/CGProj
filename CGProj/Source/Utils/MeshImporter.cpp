#include "MeshImporter.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Core/Components/TriangleComponent.h"
#include "Core/Objects/Mesh.h"


Mesh* MeshImporter::ImportMesh(const std::string& pathModel, const wchar_t* pathTex, bool centering, uint32_t numMesh)
{
    Assimp::Importer importer;
    const aiScene* sce = importer.ReadFile(pathModel, aiProcess_Triangulate | aiProcess_GenBoundingBoxes);
    if (!sce) return nullptr;
    /*printf("Meshes %d\n", sce->mNumMeshes);
    printf("Meshes %d\n", sce->mRootNode->mNumMeshes);*/
    Mesh* resMesh = Engine::GetInstance().CreateObject<Mesh>();
    resMesh->SetTexture(pathTex);
    FillMeshData(sce, resMesh, centering, numMesh);

    return resMesh;
}

bool MeshImporter::ImportMesh(const std::string& pathModel, Mesh* resMesh, bool centering, uint32_t numMesh)
{
    Assimp::Importer importer;
    const aiScene* sce = importer.ReadFile(pathModel, aiProcess_Triangulate | aiProcess_GenBoundingBoxes);
    if (!sce) return false;
    FillMeshData(sce, resMesh, centering, numMesh);
    return true;
}

void MeshImporter::FillMeshData(const aiScene* sce, Mesh* resMesh, bool centering, uint32_t numMesh)
{
    if (!resMesh) return;

    int32_t numVertices = 0;
    for (uint32_t i = 0; i < (sce->mNumMeshes && numMesh && numMesh <= sce->mNumMeshes ? numMesh : sce->mNumMeshes); i++)
    {
        const auto mesh = sce->mMeshes[i];
        for (uint32_t j = 0; j < mesh->mNumVertices; j++)
            numVertices++;
    }
    int32_t sideSize = (int32_t)std::sqrt(numVertices) + 1;

    int32_t count = 0;
    Vector3 maxAABB = Vector3::Zero;
    Vector3 minAABB = Vector3::Zero;
    Array<Vertex> importingVert;
    for (uint32_t i = 0; i < (sce->mNumMeshes && numMesh && numMesh <= sce->mNumMeshes ? numMesh : sce->mNumMeshes); i++)
    {
        const auto mesh = sce->mMeshes[i];
        printf("Vertex %d\n", mesh->mNumVertices);
        Vector3::Max(maxAABB, Vector3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z), maxAABB);
        Vector3::Min(minAABB, Vector3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z), minAABB);
        for (uint32_t j = 0; j < mesh->mNumVertices; j++)
        {
            Vertex newVert = {Vector3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z),
                              mesh->mTangents ? Vector3(mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z) : Vector3::Zero, 
                              mesh->mBitangents ? Vector3(mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z) : Vector3::Zero,
                              Vector3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z),
                              mesh->HasTextureCoords(0)
                                  ? Vector2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y)
                                  : Vector2((float)((count + j) % sideSize) / (float)sideSize, (float)((count + j) / sideSize) /(float)sideSize)
            };
            /*printf("x = %f, y = %f\n", newVert.texture.x,
                newVert.texture.y);*/
            //Vector4(color.r, color.g, color.b, color.a)};
            importingVert.insert(newVert);
        }

        count += mesh->mNumVertices;
    }
    for (uint32_t i = 0; i < (sce->mNumMeshes && numMesh && numMesh <= sce->mNumMeshes  ? numMesh : sce->mNumMeshes); i++)
    {
        const auto mesh = sce->mMeshes[i];
        printf("Vertex %d\n", mesh->mNumVertices);
        Vector3::Max(maxAABB, Vector3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z), maxAABB);
        Vector3::Min(minAABB, Vector3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z), minAABB);
        for (uint32_t j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            for (uint32_t k = 0; k < face.mNumIndices; k++)
            {
                resMesh->AddIndex(face.mIndices[k]);
            }
        }
    }
    if (centering)
    {
        const auto center = minAABB+(maxAABB - minAABB) *0.5f;
        if (center != Vector3::Zero)
            for (auto& vert : importingVert)
            {
                vert.position -= Vector3(center.x, center.y, center.z);
            }
        maxAABB -= center;
        minAABB -= center;
    }
    resMesh->SetVertices(importingVert);
    resMesh->SetAABB(minAABB, maxAABB);
}
