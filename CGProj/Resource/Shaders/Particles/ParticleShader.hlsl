#ifndef PARTICLE
#define PARTICLE

#include "../Structures.hlsl"
#include "GlobalsParticles.hlsl"

StructuredBuffer<ParticleB> Particles : register(t0);

Texture2D DiffuseTex : register( t2 );
sampler LinearRepeatSampler : register( s0 );

cbuffer ViewBuffer : register(b0)
{
ViewData viewData;
}


struct PixelInput
{
    float4 pos : SV_POSITION;
    float2 UV : TEXTURE0;
};

PixelInput VS(uint id: SV_VertexID)
{
    PixelInput output = (PixelInput)0;

    const ParticleB particle = Particles[id];
    output.pos = mul(float4(particle.pos, 1.0f), viewData.mWorld);
    output.pos = mul(output.pos, viewData.mView);
    output.UV = 0;
    return output;
}

PixelInput _offsetNprojected(PixelInput data, float2 offset, float2 uv)
{
    data.pos.xy += offset;
    data.pos = mul(data.pos, viewData.mProj);
    data.UV = uv;

    return data;
}

[maxvertexcount(4)]
void GS(point PixelInput input[1], inout TriangleStream<PixelInput> stream)
{
    PixelInput pointOut = input[0];

    const float size = 0.1f;
    stream.Append(_offsetNprojected(pointOut, float2(-1, -1) * size, float2(0, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(-1, 1) * size, float2(0, 1)));
    stream.Append(_offsetNprojected(pointOut, float2(1, -1) * size, float2(1, 0)));
    stream.Append(_offsetNprojected(pointOut, float2(1, 1) * size, float2(1, 1)));
    stream.RestartStrip();
}

float4 PS(PixelInput input) : SV_Target
{
    float4 col = DiffuseTex.Sample(LinearRepeatSampler, input.UV);
    return col;
}

#endif
