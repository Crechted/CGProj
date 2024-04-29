#ifndef POST_PROCESS_SHADER
#define POST_PROCESS_SHADER

#include "Structures.hlsl"

Texture2D shaderTextures : register(t0);
SamplerState SampleType : register(s0);

PS_IN VS(uint id: SV_VertexID)
{
    PS_IN output = (PS_IN)0;

    output.tex = float2(id & 1, (id & 2) >> 1);
    output.pos = float4(output.tex * float2(2, -2) + float2(-1, 1), 0, 1);
    //output.pos = float4(output.tex, 0.0f, 1.0f);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 col = shaderTextures.Sample(SampleType, input.tex);
    //const float GrayscaleValue = 0.2989 * col.x + 0.5870 * col.y + 0.1140 * col.z;
    //const float color = GrayscaleValue;
    const float4 color = col*2.0f;
    return color;
}

#endif

