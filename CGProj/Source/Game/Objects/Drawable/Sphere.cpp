#include "Sphere.h"

#include "Core/Components/SceneComponent.h"
#include "Core/CoreTypes.h"
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
    Array<Vertex> vertices;
    Array<int32_t> indexes;
    CreateSphere(initPos, radius, stackCount, sliceCount, vertices, indexes);
    SetVertices(vertices);
    SetIndices(indexes);
}

void Sphere::CreateSphere(Vector3 initPos, float radius, uint32_t stackCount, uint32_t sliceCount, Array<Vertex>& vertices,
    Array<int32_t>& indexes)
{
    auto phiStep = Pi / stackCount;
    auto thetaStep = 2.0f * Pi / sliceCount;

    vertices.insert(Vertex{Vector3(initPos.x, initPos.y + radius, initPos.z),
                           Vector3(1.0f, 0.0f, 0.0f),
                           Vector3::Zero,
                           Vector3(0.0f, 1.0f, 0.0f),
                           Vector2(0.5f, 0.0f)});

    for (uint32_t i = 1; i <= stackCount - 1; i++)
    {
        auto phi = i * phiStep;
        for (uint32_t j = 0; j <= sliceCount; j++)
        {
            const auto theta = j * thetaStep;
            const auto pos = Vector3(
                static_cast<float>(initPos.x + radius * sin(phi) * cos(theta)),
                static_cast<float>(initPos.y + radius * cos(phi)),
                static_cast<float>(initPos.z + radius * sin(phi) * sin(theta))
                );
            auto tang = Vector3(static_cast<float>(-radius * sin(phi) * sin(theta)), 0, static_cast<float>(radius * sin(phi) * cos(theta)));
            auto norm = pos;
            norm.Normalize();
            const auto tex = Vector2(theta / (Pi * 2), phi / Pi);
            vertices.insert(Vertex{pos, tang, Vector3::Zero, norm, tex});
        }
    }
    vertices.insert(Vertex{Vector3(initPos.x, initPos.y - radius, initPos.z),
                           Vector3(-1.0f, 0.0f, 0.0f),
                           Vector3::Zero,
                           Vector3(0.0f, -1.0f, 0.0f),
                           Vector2(0.5f, 1.0f)});

    for (uint32_t i = 1; i <= sliceCount; i++)
    {
        indexes.insert(0);
        indexes.insert(i + 1);
        indexes.insert(i);
    }

    auto baseIndex = 1;
    const auto ringVertexCount = sliceCount + 1;
    for (uint32_t i = 0; i < stackCount - 2; i++)
    {
        for (uint32_t j = 0; j < sliceCount; j++)
        {
            indexes.insert(baseIndex + (i + 1) * ringVertexCount + j);
            indexes.insert(baseIndex + i * ringVertexCount + j);
            indexes.insert(baseIndex + i * ringVertexCount + j + 1);

            indexes.insert(baseIndex + (i + 1) * ringVertexCount + j + 1);
            indexes.insert(baseIndex + (i + 1) * ringVertexCount + j);
            indexes.insert(baseIndex + i * ringVertexCount + j + 1);

        }
    }

    const auto southPoleIndex = vertices.size() - 1;
    baseIndex = southPoleIndex - ringVertexCount;
    for (uint32_t i = 0; i < sliceCount; i++)
    {
        indexes.insert(southPoleIndex);
        indexes.insert(baseIndex + i);
        indexes.insert(baseIndex + i + 1);
    }

}

void Sphere::CreateDrawSphere(Vector3 initPos, float radius, uint32_t stackCount, uint32_t sliceCount, Vector4 color,
    Array<VertexNoTex>& vertices, Array<int32_t>& indexes, bool fill)
{

    auto phiStep = Pi / stackCount;
    auto thetaStep = 2.0f * Pi / sliceCount;

    vertices.insert(VertexNoTex{Vector4(initPos.x, initPos.y + radius, initPos.z, 1.0f), color});

    for (uint32_t i = 1; i <= stackCount - 1; i++)
    {
        auto phi = i * phiStep;
        for (uint32_t j = 0; j <= sliceCount; j++)
        {
            auto theta = j * thetaStep;
            auto pos = Vector4(
                static_cast<float>(initPos.x + radius * sin(phi) * cos(theta)),
                static_cast<float>(initPos.y + radius * cos(phi)),
                static_cast<float>(initPos.z + radius * sin(phi) * sin(theta)),
                1.0f
                );
            //var t = new Vector3(-radius*MathF.Sin(phi)*MathF.Sin(theta), 0, radius*MathF.Sin(phi)*MathF.Cos(theta)); - tangent
            auto norm = pos;
            norm.Normalize();
            vertices.insert(VertexNoTex{pos, color});
        }
    }

    vertices.insert(VertexNoTex{Vector4(initPos.x, initPos.y - radius, initPos.z, 1.0f), color});

    if (fill)
    {
        for (uint32_t i = 1; i <= sliceCount; i++)
        {
            indexes.insert(0);
            indexes.insert(i + 1);
            indexes.insert(i);
        }

        auto baseIndex = 1;
        auto ringVertexCount = sliceCount + 1;
        for (uint32_t i = 0; i < stackCount - 2; i++)
        {
            for (uint32_t j = 0; j < sliceCount; j++)
            {
                indexes.insert(baseIndex + (i + 1) * ringVertexCount + j);
                indexes.insert(baseIndex + i * ringVertexCount + j);
                indexes.insert(baseIndex + i * ringVertexCount + j + 1);
                indexes.insert(baseIndex + (i + 1) * ringVertexCount + j + 1);
                indexes.insert(baseIndex + (i + 1) * ringVertexCount + j);
                indexes.insert(baseIndex + i * ringVertexCount + j + 1);

            }
        }

        auto southPoleIndex = vertices.size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;
        for (uint32_t i = 0; i < sliceCount; i++)
        {
            indexes.insert(southPoleIndex);
            indexes.insert(baseIndex + i);
            indexes.insert(baseIndex + i + 1);
        }
    }
    else
    {
        const uint32_t quarterSlice = sliceCount / 4;
        const uint32_t halfStack = stackCount / 2;

        for (uint32_t i = 1; i <= sliceCount; i += quarterSlice)
        {
            indexes.insert(0);
            indexes.insert(i);
        }

        auto baseIndex = 1;
        auto ringVertexCount = sliceCount + 1;
        for (uint32_t i = 0; i < stackCount - 2; ++i)
        {
            for (uint32_t j = 0; j < sliceCount; j += quarterSlice)
            {
                indexes.insert(baseIndex + (i + 1) * ringVertexCount + j);
                indexes.insert(baseIndex + i * ringVertexCount + j);
            }
        }

        for (uint32_t j = 0; j < sliceCount; j++)
        {
            indexes.insert(baseIndex + (halfStack - 1) * ringVertexCount + j + 1);
            indexes.insert(baseIndex + (halfStack - 1) * ringVertexCount + j);
        }

        auto southPoleIndex = vertices.size() - 1;
        baseIndex = southPoleIndex - ringVertexCount;
        for (uint32_t i = 0; i < sliceCount; i += quarterSlice)
        {
            indexes.insert(southPoleIndex);
            indexes.insert(baseIndex + i);
        }
    }
}
