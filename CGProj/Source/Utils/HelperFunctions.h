#pragma once
#include <random>

#include "Core/CoreTypes.h"

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

template <typename T>
static T Rand(T min, T max)
{
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<T> dist(min, max);
    return dist(gen);
}

static Vector2 Rand(Vector2 min, Vector2 max)
{
    Vector2 res;
    res.x = Rand(min.x, max.x);
    res.y = Rand(min.y, max.y);
    return res;
}

static Vector3 Rand(Vector3 min, Vector3 max)
{
    Vector3 res;
    res.x = Rand(min.x, max.x);
    res.y = Rand(min.y, max.y);
    res.z = Rand(min.z, max.z);
    return res;
}

static Vector4 Rand(Vector4 min, Vector4 max)
{
    Vector4 res;
    res.x = Rand(min.x, max.x);
    res.y = Rand(min.y, max.y);
    res.z = Rand(min.z, max.z);
    res.w = Rand(min.w, max.w);
    return res;
}

static float RandomVariance(float median, float variance)
{
    //float fUnitRandomValue = Rand(0.0f, 1.0f);
    float fUnitRandomValue = (float)rand() / (float)RAND_MAX;
    float fRange = variance * fUnitRandomValue;
    return median - variance + (2.0f * fRange);
}
