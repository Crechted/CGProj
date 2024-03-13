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