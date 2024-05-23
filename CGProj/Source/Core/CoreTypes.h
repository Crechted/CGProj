#pragma once
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 position = Vector3::Zero;
    Vector3 tangent = Vector3::Zero;
    Vector3 binormal = Vector3::Zero;
    Vector3 normal = Vector3::Zero;
    alignas(16) Vector2 texCoord = Vector2::Zero;
};

struct VertexNoTex
{
    Vector4 position = Vector4::Zero;
    Vector4 color = Vector4::Zero;
};

static constexpr float Pi = 3.14159265f;
