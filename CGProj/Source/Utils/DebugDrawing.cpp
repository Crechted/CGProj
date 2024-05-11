#include "DebugDrawing.h"

#include "Core/CoreTypes.h"

void DebugDrawing::CreateDrawConeByTopology(Vector3 pos, float range, float lenght, int32_t density, Vector4 color, Matrix transform,
    Array<VertexNoTex>& vertices, Array<int32_t>& indexes,
    bool fill)
{
    double angleStep = Pi * 2 / density;
    double radius = tan(range) * lenght;

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

    if (fill)
    {
        for (int32_t i = 0; i < density; i++)
        {
            indexes.insert(0);
            indexes.insert(i + 1);
            indexes.insert(i + 2);

            indexes.insert(vertices.size()-1);
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

            indexes.insert(vertices.size()-1);
            indexes.insert(i + 1);

            indexes.insert(i + 1);
            indexes.insert(i + 2);
        }
    }
}
