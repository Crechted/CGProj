#ifndef SHADOW_MAPPING_SHADER
#define SHADOW_MAPPING_SHADER

#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
LightData lightData;
}

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);

    output.pos = mul(output.pos, viewData.mViewProj);
    return output;
}

#endif
