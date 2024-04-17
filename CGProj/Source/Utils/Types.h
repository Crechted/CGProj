#pragma once

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
