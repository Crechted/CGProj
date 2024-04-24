#ifndef SHADOW_MAPPING_SHADER
#define SHADOW_MAPPING_SHADER

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

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.tex = input.tex;
    output.norm = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld);
    
    output.pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    output.pos += output.norm * sin(output.pos.x * 10.0f * lightData.kaSpecPowKsX.w);

    output.worldPos = output.pos;
    output.pos = mul(output.pos, viewData.mView);
    output.pos = mul(output.pos, viewData.mProj);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 diffVal = DiffuseMap.Sample(DiffSamp, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);
    
    return diffVal;
}

#endif
