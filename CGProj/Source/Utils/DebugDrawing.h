#pragma once
#include "SimpleMath.h"
#include "Array/Array.h"
#include "Core/CoreTypes.h"

struct VertexNoTex;
using namespace DirectX::SimpleMath;

class DebugDrawing
{
public:
    static void CreateDrawCone(Vector3 pos, float angle, float lenght, int32_t density, Vector4 color, Matrix transform,
        Array<VertexNoTex>& vertices, Array<int32_t>& indexes, bool fill = false);
    static void CreateCone(Vector3 pos, float angle, float lenght, int32_t density, Matrix transform, Array<Vertex>& vertices,
        Array<int32_t>& indexes, bool fill = true);
    static void CreateFullQuad(Vector2 dimension, Array<Vertex>& vertices, Array<int32_t>& indexes);
    static void CreateFullQuad(Vector2 dimension, Array<VertexNoTex>& vertices, Array<int32_t>& indexes);

private:
    static void GetIndexesCone(int32_t density, int32_t numVert, Array<int32_t>& indexes, bool fill);
    static void CreateIndexesFullQuad(Array<int32_t>& indexes);
};
