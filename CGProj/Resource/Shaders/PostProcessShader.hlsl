#ifndef POST_PROCCESS_SHADER
#include "Structures.hlsl"
#define

Texture2D shaderTextures;
SamplerState SampleType;

PS_IN VS(uint id: SV_VertexID)
{
    PS_IN output = (PS_IN)0;

    output.tex = float2(id & 1, (id & 2) >> 1);
    output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);

float4 PSMain(PS_IN input) : SV_Target
{
    return shaderTextures.Sample(SampleType, input.tex);
}


#endif
