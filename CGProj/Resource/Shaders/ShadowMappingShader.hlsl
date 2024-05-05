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

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    output.pos += output.norm * sin(output.pos.x * 10.0f * lightData.kaSpecPowKsX.w);

    output.pos = mul(output.pos, viewData.mViewProj);
    //output.pos = mul(output.pos, viewData.mProj);
    return output;
}

#endif
