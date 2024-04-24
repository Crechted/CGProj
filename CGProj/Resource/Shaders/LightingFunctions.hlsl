#ifndef LIGHTING_FUNCTIONS
#define LIGHTING_FUNCTIONS

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH)
{
    // perspective division
    shadowPosH.xyz /= shadowPosH.w;

    // Depth value of NDC space
    float depth = shadowPosH.z - 0.001f;

    // The width and height of the texel in texture coordinates
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    /*const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }*/

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy, depth, int2(i % 3 - 1, i / 3 - 1)).r;
    }

    return percentLit /= 9.0f;
}

float CalcShadowFactor(SamplerState samShadow, Texture2D shadowMap, float4 shadowPosH)
{

    // Sample the shadow map to get the depth value closest to the light source
    float s0 = shadowMap.Sample(samShadow, shadowPosH.xy).r;
    float s1 = shadowMap.Sample(samShadow, shadowPosH.xy + float2(SMAP_DX, 0)).r;
    float s2 = shadowMap.Sample(samShadow, shadowPosH.xy + float2(0, SMAP_DX)).r;
    float s3 = shadowMap.Sample(samShadow, shadowPosH.xy + float2(SMAP_DX, SMAP_DX)).r;

    float depth = shadowPosH.z - 0.001f;
    // Is the depth value of the pixel less than or equal to the depth value in the shadow image
    float r0 = (depth <= s0);
    float r1 = (depth <= s1);
    float r2 = (depth <= s2);
    float r3 = (depth <= s3);

    // Transform to texel space
    float2 texelPos = SMAP_SIZE * shadowPosH.xy;

    // Determine the interpolation coefficient (frac() returns the decimal part of a floating point number)
    float2 t = frac(texelPos);

    // Perform bilinear interpolation on the comparison result
    return lerp(lerp(r0, r1, t.x), lerp(r2, r3, t.x), t.y);
}

#endif
