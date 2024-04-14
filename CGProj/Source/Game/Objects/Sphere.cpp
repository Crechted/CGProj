#include "Sphere.h"

#include "Core/Components/SceneComponent.h"
#include "Utils/Types.h"
using namespace DirectX::SimpleMath;

Sphere::Sphere(const wchar_t* pathTex, Vector3 position, float radius, int32_t sliceCount, int32_t stackCount, bool drawFirstHalf,
    bool drawSecondHalf,
    bool drawUp, bool drawDown)
    : radius(radius)
      //, initPos(position)
      , sliceCount(sliceCount)
      , stackCount(stackCount)
      , drawFirstHalf(drawFirstHalf)
      , drawSecondHalf(drawSecondHalf)
      , drawUp(drawUp)
      , drawDown(drawDown)
{
    if (sceneComp) sceneComp->initPosition = position;
    initPathTex = pathTex;
}

void Sphere::Initialize()
{
    InitSphere();
    maxAABB = sceneComp->GetLocation() + Vector3(radius);
    minAABB = sceneComp->GetLocation() - Vector3(radius);
    Mesh::Initialize();
}

void Sphere::InitSphere()
{
    auto phiStep = Pi / stackCount;
    auto thetaStep = 2.0f * Pi / sliceCount;

    AddVertex(Vertex{DirectX::XMFLOAT4(initPos.x, initPos.y + radius, initPos.z, 1.0f),
                     DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
                     DirectX::XMFLOAT2(0.5f, 0.0f)});

    for (int i = 1; i <= stackCount - 1; i++)
    {
        auto phi = i * phiStep;
        for (int j = 0; j <= sliceCount; j++)
        {
            auto theta = j * thetaStep;
            auto pos = Vector4(
                initPos.x + radius * sin(phi) * cos(theta),
                initPos.y + radius * cos(phi),
                initPos.z + radius * sin(phi) * sin(theta),
                1.0f
                );
            //var t = new Vector3(-radius*MathF.Sin(phi)*MathF.Sin(theta), 0, radius*MathF.Sin(phi)*MathF.Cos(theta)); - tangent
            auto norm = pos;
            norm.Normalize();
            auto tex = Vector2(1.0f - theta / (Pi * 2), phi / Pi);
            AddVertex(Vertex{pos, norm, tex});
        }
    }
    AddVertex(Vertex{DirectX::XMFLOAT4(initPos.x, initPos.y - radius, initPos.z, 1.0f),
                     DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f),
                     DirectX::XMFLOAT2(0.5f, 1.0f)});

    if (drawUp)
        for (int i = 1; i <= sliceCount; i++)
        {
            AddIndex(0);
            AddIndex(i);
            AddIndex(i + 1);
        }

    auto baseIndex = 1;
    auto ringVertexCount = sliceCount + 1;
    for (int i = 0; i < stackCount - 2; i++)
    {
        for (int j = 0; j < sliceCount; j++)
        {
            if (drawFirstHalf)
            {
                AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
                AddIndex(baseIndex + i * ringVertexCount + j);
                AddIndex(baseIndex + i * ringVertexCount + j + 1);
            }
            if (drawSecondHalf)
            {
                AddIndex(baseIndex + (i + 1) * ringVertexCount + j);
                AddIndex(baseIndex + (i + 1) * ringVertexCount + j + 1);
                AddIndex(baseIndex + i * ringVertexCount + j + 1);
            }
        }
    }

    if (drawDown)
    {
        auto southPoleIndex = GetNumVertices() - 1;
        baseIndex = southPoleIndex - ringVertexCount;
        for (int i = 0; i < sliceCount; i++)
        {
            AddIndex(southPoleIndex);
            AddIndex(baseIndex + i);
            AddIndex(baseIndex + i + 1);
        }
    }
}
