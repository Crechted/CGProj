#ifndef SHADOW_MAPPING_SHADER
#define SHADOW_MAPPING_SHADER

#ifndef DO_CASCADE_SHADOW
#define DO_CASCADE_SHADOW
#endif

#ifdef DO_CASCADE_SHADOW
#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
DirectionLightData lightData;
}

cbuffer CascadeBuf : register(b2)
{
CascadeData CascData;
}

GS_IN VS(VS_IN input)
{
    GS_IN output = (GS_IN)0;

    output.pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    //output.pos += sin(output.pos.x * 10.0f * lightData.kaSpecPowKsX.w);
    return output;
}

[instance(CASCADE_COUNT)]
[maxvertexcount(3)]
void GS(triangle GS_IN p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GS_OUT> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GS_OUT gs = (GS_OUT)0;
        gs.pos = mul(float4(p[i].pos.xyz, 1.0f), CascData.ViewProj[id]);
        gs.arrInd = id;
        stream.Append(gs);
    }
}

#endif
#endif
