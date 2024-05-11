#include "Box.h"
#include "Core/Components/SceneComponent.h"
#include "Core/Components/TriangleComponent.h"
#include "Core/CoreTypes.h"

Box::Box(const wchar_t* pathTex, Vector3 position, Vector3 size, Vector4 color, D3D_PRIMITIVE_TOPOLOGY topology)
    : initSize(size)
      , initColor(color)
      , topology(topology)
{
    if (sceneComp) sceneComp->initPosition = position;
    initPathTex = pathTex;
}

void Box::InitMesh()
{
    InitBox(initSize, topology);
}

void Box::InitBox(Vector3 size, D3D_PRIMITIVE_TOPOLOGY topology)
{
    triangleComp->topology = topology;
    Array<Vertex> vertices;
    Array<int32_t> indexes;
    CreateBoxByTopology(size, vertices, indexes, topology);
    SetVertices(vertices);
    SetIndices(indexes);
}

void Box::CreateBoxByTopology(Vector3 size, Array<Vertex>& vertices, Array<int32_t>& indexes, D3D_PRIMITIVE_TOPOLOGY topology)
{
    auto w2 = 0.5f * size.x;
    auto h2 = 0.5f * size.y;
    auto d2 = 0.5f * size.z;

    const float invU = 0.0f;
    vertices.insert(Vertex{Vector3(-w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, -1.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)}); // front
    vertices.insert(Vertex{Vector3(-w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, -1.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, -1.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, -1.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)});
    vertices.insert(Vertex{Vector3(-w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, 1.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)}); // back
    vertices.insert(Vertex{Vector3(+w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, 1.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)});
    vertices.insert(Vertex{Vector3(+w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, 1.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 0.0f, 1.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)}); // top
    vertices.insert(Vertex{Vector3(-w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, 1.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)});
    vertices.insert(Vertex{Vector3(-w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, -1.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)}); // bottom
    vertices.insert(Vertex{Vector3(+w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, -1.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)});
    vertices.insert(Vertex{Vector3(+w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, -1.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(0.0f, -1.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(-1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)}); // left
    vertices.insert(Vertex{Vector3(-w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(-1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(-1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(-w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(-1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)});
    vertices.insert(Vertex{Vector3(+w2, -h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 1.0f)}); // right
    vertices.insert(Vertex{Vector3(+w2, +h2, -d2), Vector3::Zero, Vector3::Zero, Vector3(1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 0.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, +h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 0.0f)});
    vertices.insert(Vertex{Vector3(+w2, -h2, +d2), Vector3::Zero, Vector3::Zero, Vector3(1.0f, 0.0f, 0.0f),
                           Vector2(abs(invU + 1.0f), 1.0f)});

    GetIndexesByTopology(indexes, topology);
}

void Box::CreateDrawBoxByTopology(Vector4 pos, Vector3 size, Vector4 color, Array<VertexNoTex>& vertices, Array<int32_t>& indexes,
    D3D_PRIMITIVE_TOPOLOGY topology)
{
    auto w2 = 0.5f * size.x;
    auto h2 = 0.5f * size.y;
    auto d2 = 0.5f * size.z;

    if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
    {
        for (int32_t x = 0; x < 2; ++x)
        {
            for (int32_t y = 0; y < 2; ++y)
            {
                for (int32_t z = 0; z < 2; ++z)
                {
                    const auto cofX = 2.0f * static_cast<float>(x) - 1.0f;
                    const auto cofY = 2.0f * static_cast<float>(y) - 1.0f;
                    const auto cofZ = 2.0f * static_cast<float>(z) - 1.0f;
                    vertices.insert(VertexNoTex{pos + Vector4(cofX * w2, cofY * h2, cofZ * d2, 1.0f), color}); // front
                }
            }
        }
        const int32_t num = 24;
        const auto inds = new int32_t[num]
        {0, 1, 1, 3,
         3, 2, 2, 0,
         0, 4, 4, 6, 6, 2,
         1, 5, 5, 7, 7, 3,
         4, 5, 6, 7,
        };
        indexes.clear();
        for (int32_t i = 0; i < num; i++)
        {
            indexes.insert(inds[i]);
        }
    }
    else
    {

        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, -d2, 1.0f), color}); // front
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, +d2, 1.0f), color}); // back
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, -d2, 1.0f), color}); // top
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, -d2, 1.0f), color}); // bottom
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, +d2, 1.0f), color}); // left
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, +h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(-w2, -h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, -d2, 1.0f), color}); // right
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, -d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, +h2, +d2, 1.0f), color});
        vertices.insert(VertexNoTex{pos + Vector4(+w2, -h2, +d2, 1.0f), color});
        GetIndexesByTopology(indexes, topology);
    }
}

void Box::GetIndexesByTopology(Array<int32_t>& indexes, D3D_PRIMITIVE_TOPOLOGY topology)
{
    if (topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
    {
        auto indx = new int32_t[36]
        {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        };
        for (int32_t i = 0; i < 36; i++)
        {
            indexes.insert(indx[i]);
        }
    }
    if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
    {
        indexes.clear();
        auto indx = new int32_t[48]
        {
            0, 1, 1, 2, 2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            8, 9, 9, 10, 10, 11, 11, 8,
            12, 13, 13, 14, 14, 15, 15, 12,
            16, 17, 17, 18, 18, 19, 19, 16,
            20, 21, 21, 22, 22, 23, 23, 20
        };
        for (int32_t i = 0; i < 48; i++)
        {
            indexes.insert(indx[i]);
        }
    }
}
