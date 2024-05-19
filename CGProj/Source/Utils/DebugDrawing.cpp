#include "DebugDrawing.h"

#include "Core/CoreTypes.h"

void DebugDrawing::CreateDrawCone(Vector3 pos, float angle, float lenght, int32_t density, Vector4 color, Matrix transform,
    Array<VertexNoTex>& vertices, Array<int32_t>& indexes,
    bool fill)
{
    double angleStep = Pi * 2 / density;
    double radius = tan(RadiansFromDegree(angle)) * lenght;

    const auto up = Vector3::Transform(pos, transform);
    vertices.insert(VertexNoTex{Vector4(up.x, up.y, up.z, 1.0f), color});
    for (int i = 0; i <= density; i++)
    {
        auto pointX = pos.x + radius * cos(angleStep * i);
        auto pointY = pos.y + radius * sin(angleStep * i);

        auto p = Vector3::Transform(Vector3(static_cast<float>(pointX), static_cast<float>(pointY), pos.z - lenght), transform);
        vertices.insert(VertexNoTex{Vector4(p.x, p.y, p.z, 1.0f), color});
    }
    const auto down = Vector3::Transform(Vector3(pos.x, pos.y, pos.z - lenght), transform);
    vertices.insert(VertexNoTex{Vector4(down.x, down.y, down.z, 1.0f), color});

    GetIndexesCone(density, vertices.size(), indexes, fill);
}

void DebugDrawing::CreateCone(Vector3 pos, float angle, float lenght, int32_t density, Matrix transform,
    Array<Vertex>& vertices, Array<int32_t>& indexes, bool fill)
{
    double angleStep = Pi * 2 / density;
    double radius = tan(RadiansFromDegree(angle)) * lenght;

    const auto up = Vector3::Transform(pos, transform);
    vertices.insert(Vertex{up});
    for (int i = 0; i <= density; i++)
    {
        auto pointX = pos.x + radius * cos(angleStep * i);
        auto pointY = pos.y + radius * sin(angleStep * i);

        auto p = Vector3::Transform(Vector3(static_cast<float>(pointX), static_cast<float>(pointY), pos.z - lenght), transform);
        vertices.insert(Vertex{p});
    }
    const auto down = Vector3::Transform(Vector3(pos.x, pos.y, pos.z - lenght), transform);
    vertices.insert(Vertex{down});

    GetIndexesCone(density, vertices.size(), indexes, fill);
}

void DebugDrawing::GetIndexesCone(int32_t density, int32_t numVert, Array<int32_t>& indexes, bool fill)
{
    if (fill)
    {
        for (int32_t i = 0; i < density; i++)
        {
            indexes.insert(0);
            indexes.insert(i + 2);
            indexes.insert(i + 1);

            indexes.insert(numVert - 1);
            indexes.insert(i + 1);
            indexes.insert(i + 2);
        }
    }
    else
    {
        for (int32_t i = 0; i < density; i++)
        {
            indexes.insert(0);
            indexes.insert(i + 1);

            indexes.insert(numVert - 1);
            indexes.insert(i + 1);

            indexes.insert(i + 1);
            indexes.insert(i + 2);
        }
    }
}

void DebugDrawing::CreateFullQuad(Vector2 dimension, Array<Vertex>& vertices, Array<int32_t>& indexes)
{
    const float centreW = dimension.x / 2 * -1;
    const float centreH = dimension.y / 2;
    const float left = centreW;
    const float right = left + dimension.x;
    const float top = centreH;
    const float bottom = top - dimension.y;

    vertices.insert({Vector3(left, top, 0.0f), Vector3::Zero, Vector3::Zero, Vector3::Zero, Vector2(0.0f, 0.0f)});
    vertices.insert({Vector3(right, top, 0.0f), Vector3::Zero, Vector3::Zero, Vector3::Zero, Vector2(1.0f, 1.0f)});
    vertices.insert({Vector3(left, bottom, 0.0f), Vector3::Zero, Vector3::Zero, Vector3::Zero, Vector2(0.0f, 1.0f)});
    vertices.insert({Vector3(right, bottom, 0.0f), Vector3::Zero, Vector3::Zero, Vector3::Zero, Vector2(1.0f, 0.0f)});

    CreateIndexesFullQuad(indexes);
}

void DebugDrawing::CreateFullQuad(Vector2 dimension, Array<VertexNoTex>& vertices, Array<int32_t>& indexes)
{
    const float centreW = dimension.x / 2 * -1;
    const float centreH = dimension.y / 2;
    const float left = centreW;
    const float right = left + dimension.x;
    const float top = centreH;
    const float bottom = top - dimension.y;

    vertices.insert({Vector4(left, top, 0.0f, 1.0f), Vector4::One});
    vertices.insert({Vector4(right, top, 0.0f, 1.0f), Vector4::One});
    vertices.insert({Vector4(left, bottom, 0.0f, 1.0f), Vector4::One});
    vertices.insert({Vector4(right, bottom, 0.0f, 1.0f), Vector4::One});

    CreateIndexesFullQuad(indexes);
}

void DebugDrawing::CreateIndexesFullQuad(Array<int32_t>& indexes)
{
    indexes.insert(0);
    indexes.insert(1);
    indexes.insert(3);
    indexes.insert(0);
    indexes.insert(3);
    indexes.insert(2);
}
