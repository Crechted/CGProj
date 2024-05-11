#pragma once
#include "SimpleMath.h"
#include "Array/Array.h"

struct VertexNoTex;
using namespace DirectX::SimpleMath;

class DebugDrawing
{
public:
    static void CreateDrawConeByTopology(Vector3 pos, float angle, float lenght, int32_t density, Vector4 color, Matrix transform, Array<VertexNoTex>& vertices, Array<int32_t>& indexes, bool fill = false);
};
