#ifndef TRIANGLE_SHADER
#define TRIANGLE_SHADER

//#define DO_CASCADE_SHADOW

#include "LightingFunctions.hlsl"
#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
DirectionLightData lightData;
}

#ifdef DO_CASCADE_SHADOW
cbuffer CascadeBuf : register(b2)
{
CascadeData CascData;
}
#endif

Texture2D DiffuseMap : register(t0);
SamplerState DiffSamp : register(s0);
Texture2D ShadowMap : register(t1);
SamplerComparisonState ShadCompSamp : register(s1);

#ifdef DO_CASCADE_SHADOW
Texture2DArray CascadeShadowMaps : register(t2);
#endif

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.norm = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld);
    output.tex = input.tex;
    float4 pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    pos += sin(pos.x * 10.0f * lightData.kaSpecPowKsX.w);

    output.worldPos = pos;
    output.pos = pos;
    output.pos = mul(output.pos, viewData.mViewProj);

    output.ShadowPosH = mul(output.worldPos, lightData.mShadowTransform);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 diffVal = DiffuseMap.Sample(DiffSamp, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);
    if (lightData.color.r == float4(0.0f, 0.0f, 0.0f, 0.0f).r) return diffVal;

    float3 kd = diffVal.xyz;
    float3 normal = normalize(input.norm.xyz);

    float3 viewDir = normalize(viewData.camPos.xyz - input.worldPos.xyz);
    float3 lightDir = -lightData.direction.xyz;
    float3 refVec = normalize(reflect(lightDir, normal));

    float3 diffuse = max(0, dot(lightDir, normal)) * kd;
    float3 ambient = kd * lightData.kaSpecPowKsX.x;
    float3 spec = pow(max(0, dot(-viewDir, refVec)), lightData.kaSpecPowKsX.y) * lightData.kaSpecPowKsX.z;

    float shadow;
    float4 lightColor = lightData.color;
    matrix mT = {
        {0.5f, 0.0f, 0.0f, 0.0f},
        {0.0f, -0.5f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}};
#ifdef DO_CASCADE_SHADOW
    uint layer = 0;
    float dephtVal = abs(input.pos.w);
    bool drawDebug = true;
    /*for (int i = 0; i < 4; ++i)
    {
        if (dephtVal < CascData.Distances[i])
            //if (dephtVal < ((float[4])CascData.Distances[i/4])[i%4])
            {
            layer = i;
            break;
            }
    }*/  
    if (dephtVal < CascData.Distances.x)
    {
        layer = 0;
        if (drawDebug) lightColor.yz = 0.2;
    }
    else if (dephtVal < CascData.Distances.y)
    {
        layer = 1;
        if (drawDebug) lightColor.xz = 0;
    }
    else if (dephtVal < CascData.Distances.z)
    {
        layer = 2;
        if (drawDebug) lightColor.xy = 0;
    }
    else if (dephtVal <= CascData.Distances.w)
    {
        layer = 3;
        if (drawDebug) lightColor.x = 0;
    }
    input.ShadowPosH = mul(input.worldPos, mul(CascData.ViewProj[layer], mT));
    shadow = CalcCascadeShadowFactor(ShadCompSamp, CascadeShadowMaps, input.ShadowPosH, layer);
#endif

#ifndef DO_CASCADE_SHADOW
    shadow = CalcShadowFactor(ShadCompSamp, ShadowMap, input.ShadowPosH);
#endif
    diffuse *= shadow;
    spec *= shadow;
    float4 col = float4(lightColor.xyz * (diffuse + ambient + spec), 1.0f);
    col.rgb = pow(col.rgb, 1.0f / 2.2f);

    return col;
}

#endif
