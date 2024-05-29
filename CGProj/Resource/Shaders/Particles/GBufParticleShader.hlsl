#ifndef GBUF_PARTICLE
#define GBUF_PARTICLE

#include "../Structures.hlsl"
#include "GlobalsParticles.hlsl"

StructuredBuffer<ParticleB> Particles : register(t0);

Texture2D DiffuseTex : register( t2 );

cbuffer ViewBuffer : register(b0)
{
ViewData viewData;
}

struct GBuffer
{
    float4 LightAccumulation : SV_Target0; // Ambient + emissive (R8G8B8_????) Unused (A8_UNORM)
    float4 Diffuse : SV_Target1;           // Diffuse Albedo (R8G8B8_UNORM) Unused (A8_UNORM)
    float4 Specular : SV_Target2;          // Specular Color (R8G8B8_UNROM) Specular Power(A8_UNORM)
    float4 NormalVS : SV_Target3;          // View space normal (R32G32B32_FLOAT) Unused (A32_FLOAT)
};

cbuffer MaterialBuf : register(b2)
{
Material material;
}

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 normWS : NORMAL;
    float2 texCoord : TEXTURE0;
};

PixelInput VS(uint id: SV_VertexID)
{
    PixelInput output = (PixelInput)0;

    const ParticleB particle = Particles[id];
    output.pos = mul(float4(particle.pos, 1.0f), viewData.mWorld);
    output.normWS = normalize(viewData.camPos-output.pos).xyz;
    output.pos = mul(output.pos, viewData.mView);
    output.texCoord = 0;
    return output;
}

PixelInput _offsetNprojected(PixelInput data, float2 offset, float2 uv)
{
    data.pos.xy += offset;

    data.pos = mul(data.pos, viewData.mProj);
    data.texCoord = uv;

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

[earlydepthstencil]
GBuffer PS(PixelInput input)
{
    //float4 col = DiffuseTex.Sample(LinearRepeatSampler, input.texCoord);

    GBuffer OUT;

    float4 diffuse = material.diffuseColor;
    if (material.hasDiffuseTex)
    {
        float4 diffuseTex = DiffuseTex.Sample(samWrapLinear, input.texCoord);
        diffuse = diffuseTex;
        /*if (any(diffuse.rgb))
        {
            diffuse *= diffuseTex;
        }
        else
        {
            diffuse = diffuseTex;
        }*/
    }

    float alpha = diffuse.a;
    if (alpha * material.opacity < material.alphaThreshold)
    {
        discard;
    }
    
    OUT.Diffuse = diffuse;
    float4 ambient = material.ambientColor;
    ambient *= material.globalAmbient;
    float4 emissive = material.emissiveColor;

    OUT.LightAccumulation = (ambient * diffuse + emissive);
    OUT.NormalVS = normalize(float4(input.normWS, 0));

    float specularPower = 0.0f;//material.specularPower;

    float4 specular = 0;
    if (specularPower > 1.0f) // If specular power is too low, don't use it.
    {
        specular = material.specularColor;
    }

    // Method of packing specular power from "Deferred Rendering in Killzone 2" presentation 
    // from Michiel van der Leeuw, Guerrilla (2007)
    OUT.Specular = float4(specular.rgb, log2(specularPower) / 10.5f);

    return OUT;
}

#endif
