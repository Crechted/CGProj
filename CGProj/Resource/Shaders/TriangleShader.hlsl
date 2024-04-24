#ifndef TRIANGLE_SHADER
#define TRIANGLE_SHADER

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

Texture2D DiffuseMap : register(t0);
SamplerState DiffSamp : register(s0);
Texture2D ShadowMap : register(t1);
SamplerComparisonState ShadCompSamp : register(s1);

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.norm = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld);
    output.tex = input.tex;
    float4 pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    pos += output.norm * sin(pos.x * 10.0f * lightData.kaSpecPowKsX.w);

    output.worldPos = pos;
    output.pos = pos;
    output.pos = mul(output.pos, viewData.mView);
    output.pos = mul(output.pos, viewData.mProj);

    output.ShadowPosH = mul(output.worldPos, viewData.mShadowTransform);
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

    float shadow = CalcShadowFactor(ShadCompSamp, ShadowMap, input.ShadowPosH);
    //float shadow = CalcShadowFactor(ShadSamp, ShadowMap, input.ShadowPosH);
    diffuse *= shadow;
    spec *= shadow;

    float4 col = float4(lightData.color.xyz * (diffuse + ambient + spec), 1.0f);
    col.rgb = pow(col.rgb, 1.0f / 2.2f);

    return col;
}

#endif
