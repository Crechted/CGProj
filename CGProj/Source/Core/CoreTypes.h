#pragma once
#include <string>
#include <strstream>

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 position = Vector3::Zero;
    Vector3 tangent = Vector3::Zero;
    Vector3 binormal = Vector3::Zero;
    Vector3 normal = Vector3::Zero;
    alignas(16) Vector2 texture = Vector2::Zero;
};

struct VertexNoTex
{
    Vector4 position = Vector4::Zero;
    Vector4 color = Vector4::Zero;
};

static constexpr float Pi = 3.14159265f;

static float RadiansFromDegree(float deg)
{
    return deg * Pi / 180;
}

static float DegreeFromRadians(float rad)
{
    return rad * 180 / Pi;
}

static Vector3 RadiansFromDegree(const Vector3& vec)
{
    const auto x = RadiansFromDegree(vec.x);
    const auto y = RadiansFromDegree(vec.y);
    const auto z = RadiansFromDegree(vec.z);
    return Vector3{x, y, z};
}

static Vector3 DegreeFromRadians(const Vector3& vec)
{
    const auto x = DegreeFromRadians(vec.x);
    const auto y = DegreeFromRadians(vec.y);
    const auto z = DegreeFromRadians(vec.z);
    return Vector3{x, y, z};
}