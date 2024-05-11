#ifndef POST_PROCESS_SHADER
#define POST_PROCESS_SHADER

#include "Structures.hlsl"

Texture2D shaderTextures : register(t0);
SamplerState SampleType : register(s0);

cbuffer DirLightBuf : register(b0)
{
    float4 time;
}

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
    float2 texCoord = input.tex.xy/* + float2(sin(input.tex.y*50.0f + time.x) * 0.01f, 0.0f)*/;
    float4 col = shaderTextures.Sample(SampleType, texCoord);
    
    const float GrayscaleValue = 0.2989 * col.x + 0.5870 * col.y + 0.1140 * col.z;
    //col = GrayscaleValue;
    //col = col*2.0f;
    return col;
}

#endif
