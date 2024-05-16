#ifndef DEFERRED_LIGHT
#define DEFERRED_LIGHT

#include "Structures.hlsl"
#include "LightingFunctions.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer ScreenToWorldParams : register( b2 )
{
matrix InverseProjView;
float3 CamPos;
float2 ScreenDimensions;
}

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

/*
PS_IN VS(uint id: SV_VertexID)
{
    PS_IN output = (PS_IN)0;

    output.texCoord = float2(id & 1, (id & 2) >> 1);
    output.pos = float4(output.texCoord * float2(2, -2) + float2(-1, 1), 0, 1);
    //output.pos = float4(output.tex, 0.0f, 1.0f);
    return output;
}
*/
PS_IN VS(float4 pos : POSITION,
    float4 col : COLOR)
{
    PS_IN output = (PS_IN)0;

    output.pos = mul(pos, viewData.mWorld);
    output.pos = mul(output.pos, viewData.mViewProj);
    return output;
}

[earlydepthstencil]
float4 PS(PS_IN input) : SV_Target
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

    ambient *= diffuse;
    // Unpack the specular power from the alpha component of the specular color.
    float specularPower = exp2(specular.a * 10.5f);

    LightData light = Lights[LightIndex];

    Material mat = (Material)0;
    mat.diffuseColor = diffuse;
    mat.specularColor = specular;
    mat.specularPower = specularPower;

    LightingResult lit = (LightingResult)0;

    switch (light.type)
    {
        case DIRECTIONAL_LIGHT:
        {
            lit = DoDirectionalLight(light, mat, V, N);
            break;
        }
        case POINT_LIGHT:
        {
            lit = DoPointLight(light, mat, V, P, N);
            break;
        }
        case SPOT_LIGHT:
        {
            lit = DoSpotLight(light, mat, V, P, N);
            break;
        }
    }

    return ambient + (diffuse * lit.diffuse) + (specular * lit.specular);
}

#endif
