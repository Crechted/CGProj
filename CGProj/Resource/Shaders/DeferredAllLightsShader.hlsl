#ifndef DEFERRED_ALL_LIGHT
#define DEFERRED_ALL_LIGHT

#include "Structures.hlsl"
#include "LightingFunctions.hlsl"

#define CASCADE_COUNT 8

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer ScreenToWorldParams : register( b2 )
{
matrix InverseProjView;
matrix ViewProj;
float3 CamPos;
float2 ScreenDimensions;
}


/*struct CascadeData
{
    matrix ViewProj[CASCADE_COUNT];
    float4 Distances[CASCADE_COUNT / 4];
};*/

#ifdef DO_CASCADE_SHADOW
cbuffer CascadeBuf : register(b3)
{
CascadeData CascData;
}
#endif

cbuffer LightIndexBuffer : register( b4 )
{
int LightIndex;
}

Texture2D LightAccumulationTextureVS : register( t0 );
Texture2D DiffuseTextureVS : register( t1 );
Texture2D SpecularTextureVS : register( t2 );
Texture2D NormalTextureVS : register( t3 );
Texture2D DepthTextureVS : register( t4 );

StructuredBuffer<LightData> Lights : register( t8 );
Texture2DArray CascadeShadowMaps : register(t10);

SamplerComparisonState ShadCompSamp : register(s1);

float4 ClipToWorld(float4 clip)
{
    float4 posWS = mul(clip, InverseProjView);
    posWS = posWS / posWS.w;

    return posWS;
}

float4 ScreenToWorld(float4 screen)
{
    float2 texCoord = screen.xy / ScreenDimensions;
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

    return ClipToWorld(clip);
}

struct PSInput
{
    float4 pos : SV_POSITION;
    uint instID : SV_InstanceID;
};

PSInput VS(uint id: SV_VertexID, uint instID: SV_InstanceID)
{
    PSInput output = (PSInput)0;

    const float2 texCoord = float2(id & 1, (id & 2) >> 1);
    output.pos = float4(texCoord * float2(2, -2) + float2(-1, 1), 0, 1);
    output.instID = instID;
    return output;
}

[earlydepthstencil]
float4 PS(PSInput input) : SV_Target
{
    float4 eyePos = float4(CamPos, 1.0f);

    int2 texCoord = input.pos.xy;
    float depth = DepthTextureVS.Load(int3(texCoord, 0)).r;

    float4 P = ScreenToWorld(float4(texCoord, depth, 1.0f));

    // View vector
    float4 V = normalize(eyePos - P);

    float4 ambient = LightAccumulationTextureVS.Load(int3(texCoord, 0));
    float4 diffuse = DiffuseTextureVS.Load(int3(texCoord, 0));
    float4 specular = SpecularTextureVS.Load(int3(texCoord, 0));
    float4 N = NormalTextureVS.Load(int3(texCoord, 0));

    // Unpack the specular power from the alpha component of the specular color.
    float specularPower = exp2(specular.a * 10.5f);

    LightData light = Lights[LightIndex];

    Material mat = (Material)0;
    mat.diffuseColor = diffuse;
    mat.specularColor = specular;
    mat.specularPower = specularPower;

    float shadow = 1.0f;
#ifdef DO_CASCADE_SHADOW
    uint layer = 0;
    float4 Pvp = mul(P, ViewProj);
    const float distance = abs(Pvp.w);
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        if (distance < ((float[4])(CascData.Distances[i / 4]))[i % 4])
        {
            layer = i;
            break;
        }
    }
    float4 shadowPosH = mul(P, mul(CascData.ViewProj[layer], mT));
    shadow = CalcCascadeShadowFactor(ShadCompSamp, CascadeShadowMaps, shadowPosH, layer);
#endif

    LightingResult lit = DoLighting(Lights[input.instID], mat, eyePos, P, N, shadow);
    return (diffuse * lit.diffuse) + (specular * lit.specular);
}

#endif
