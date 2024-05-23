#ifndef LIGHTING_FUNCTIONS
#define LIGHTING_FUNCTIONS

#include "Structures.hlsl"
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

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy, depth, int2(i % 3 - 1, i / 3 - 1)).r;
    }

    return percentLit /= 9.0f;
}

float CalcCascadeShadowFactor(SamplerComparisonState samShadow, Texture2DArray shadowMap, float4 shadowPosH, uint idx)
{
    shadowPosH.xyz /= shadowPosH.w;

    float depth = shadowPosH.z - 0.001f;

    float percentLit = 0.0f;

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            float3(shadowPosH.xy, idx), depth, int2(i % 3 - 1, i / 3 - 1)).r;
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

float4 DoDiffuse(LightData light, float4 L, float4 N)
{
    const float NdotL = max(dot(L, N), 0);
    return light.color * NdotL;
}

float4 DoSpecular(LightData light, Material mat, float4 V, float4 L, float4 N)
{
    float4 R = normalize(reflect(-L, N));
    const float RdotV = max(dot(R, V), 0);
    return light.color * pow(RdotV, mat.specularPower);
}

float DoAttenuation(LightData light, float d)
{
    return 1.0f - smoothstep(light.range * 0.75f, light.range, d);
}

struct LightingResult
{
    float4 diffuse;
    float4 specular;
};

LightingResult DoPointLight(LightData light, Material mat, float4 V, float4 P, float4 N)
{
    LightingResult result;

    //float4 L = light.posVS - P;
    float4 L = light.posWS - P;
    const float distance = length(L);
    L = L / distance;
    const float attenuation = DoAttenuation(light, distance);

    result.diffuse = DoDiffuse(light, L, N) * attenuation;
    result.specular = DoSpecular(light, mat, V, L, N) * attenuation;

    return result;
}

float DoSpotCone(LightData light, float4 L)
{
    const float minCos = cos(radians(light.spotlightAngle));
    const float maxCos = lerp(minCos, 1, 0.5f);
    //const float cosAngle = dot(light.directionVS, -L);
    const float cosAngle = dot(light.directionWS, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(LightData light, Material mat, float4 V, float4 P, float4 N)
{
    LightingResult result;

    //float4 L = light.posVS - P;
    float4 L = light.posWS - P;
    const float distance = length(L);
    L = L / distance;

    const float attenuation = DoAttenuation(light, distance);
    const float spotIntensity = DoSpotCone(light, L);

    result.diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.specular = DoSpecular(light, mat, V, L, N) * attenuation * spotIntensity;

    return result;
}


LightingResult DoDirectionalLight(LightData light, Material mat, float4 V, float4 N)
{
    LightingResult result;
    //float4 L = normalize(-light.directionVS);
    float4 L = normalize(-light.directionWS);

    result.diffuse = DoDiffuse(light, L, N);
    result.specular = DoSpecular(light, mat, V, L, N);

    return result;
}

LightingResult DoLighting(StructuredBuffer<LightData> lights, Material mat, float4 eyePos, float4 P, float4 N, float shadowDir = 1.0f)
{
    float4 V = normalize(eyePos - P);

    LightingResult totalResult = (LightingResult)0;

    uint count;
    uint stride;
    lights.GetDimensions(count, stride);
    //[unroll]
    for (uint i = 0; i < count ; ++i)
    {
        LightingResult result = (LightingResult)0;
        
        if (lights[i].enabled == 0)
            continue;

        if (lights[i].type != DIRECTIONAL_LIGHT && length(lights[i].posWS - P) > lights[i].range)
            continue;

        switch (lights[i].type)
        {
            case DIRECTIONAL_LIGHT:
            {
                result = DoDirectionalLight(lights[i], mat, V, N);
                result.diffuse *= shadowDir;
                result.specular *= shadowDir;
            }
            break;
            case POINT_LIGHT:
            {
                result = DoPointLight(lights[i], mat, V, P, N);
            }
            break;
            case SPOT_LIGHT:
            {
                result = DoSpotLight(lights[i], mat, V, P, N);
            }
            break;
        }
        totalResult.diffuse += result.diffuse;
        totalResult.specular += result.specular;
    }
    return totalResult;
}


#endif
