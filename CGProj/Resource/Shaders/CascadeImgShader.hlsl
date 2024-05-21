#ifndef FULL_QUAD_SHADER
#define FULL_QUAD_SHADER

#include "Structures.hlsl"

Texture2DArray shaderTextures : register(t0);
SamplerState SampleType : register(s0);

cbuffer DirLightBuf : register(b0)
{
    uint indx;
}

PS_IN VS(uint id: SV_VertexID)
{
    PS_IN output = (PS_IN)0;

    output.texCoord = float2(id & 1, (id & 2) >> 1);
    output.pos = float4(output.texCoord * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    //float4 col = shaderTextures.Load(float4(input.texCoord.xy, indx ,0));
    float4 col = shaderTextures.Sample(SampleType, float4(input.texCoord.xy, indx ,0));
    const float GrayscaleValue = 0.2989 * col.x + 0.5870 * col.y + 0.1140 * col.z;
    return GrayscaleValue;
}

#endif
