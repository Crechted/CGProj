#ifndef TRIANGLE_SHADER
#define TRIANGLE_SHADER

#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

Texture2D DiffuseTex : register( t2 );
SamplerState DiffSamp : register(s0);

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.tangWS = mul(float4(input.tang.xyz, 0.0f), viewData.mWorld).xyz;
    output.binormWS = mul(float4(input.binorm.xyz, 0.0f), viewData.mWorld).xyz;
    output.normWS = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld).xyz;
    output.texCoord = float2(input.texCoord.x, 1.0f - input.texCoord.y);

    output.posWS = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    output.pos = mul(output.posWS, viewData.mViewProj);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 diffVal = DiffuseTex.Sample(DiffSamp, input.texCoord);
    clip(diffVal.a - 0.01f);

    return diffVal;
}

#endif
