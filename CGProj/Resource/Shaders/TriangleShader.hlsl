#ifndef TRIANGLE_SHADER
#define TRIANGLE_SHADER

//#define DO_CASCADE_SHADOW

#include "LightingFunctions.hlsl"
#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer LightBuf : register(b1)
{
LightData lightData;
}

cbuffer MaterialBuf : register(b2)
{
Material material;
}

#ifdef DO_CASCADE_SHADOW
cbuffer CascadeBuf : register(b3)
{
CascadeData CascData;
}
#endif

Texture2D AmbientTex : register( t0 );
Texture2D EmissiveTex : register( t1 );
Texture2D DiffuseTex : register( t2 );
Texture2D SpecularTex : register( t3 );
Texture2D SpecularPowerTex : register( t4 );
Texture2D NormalTex : register( t5 );
Texture2D BumpTex : register( t6 );
Texture2D OpacityTex : register( t7 );

StructuredBuffer<LightData> Lights : register( t8 );

SamplerState DiffSamp : register(s0);
Texture2D ShadowMap : register(t9);
SamplerComparisonState ShadCompSamp : register(s1);

#ifdef DO_CASCADE_SHADOW
Texture2DArray CascadeShadowMaps : register(t10);
#endif

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.tangWS = mul(float4(input.tang.xyz, 0.0f), viewData.mWorld).xyz;
    output.binormWS = mul(float4(input.binorm.xyz, 0.0f), viewData.mWorld).xyz;
    output.normWS = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld).xyz;
    output.tex = input.texCoord;
    float4 pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);

    //pos += sin(pos.x * 10.0f * lightData.kaSpecPowKsX.w);
    output.posWS = pos;
    output.pos = pos;
    output.pos = mul(output.pos, viewData.mViewProj);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 diffVal = DiffuseTex.Sample(DiffSamp, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);

    float3 kd = diffVal.xyz;
    float3 normal = normalize(input.normWS.xyz);

    /*LightData light = Lights[0];

    float3 viewDir = normalize(viewData.camPos.xyz - input.posWS.xyz);
    float3 ambient = kd * material.globalAmbient;
    float3 lightDir = -light.directionWS.xyz;
    float3 diffuse = max(0, dot(lightDir, normal)) * kd;

    float3 refVec = normalize(reflect(lightDir, normal));
    float3 specular = pow(max(0, dot(-viewDir, refVec)), material.specularPower) * material.specularScale;*/

    float4 emissive = material.emissiveColor;
    float4 ambient = material.globalAmbient;
    float4 diffuse = diffVal;
    float4 specular;
    ambient *= diffVal;
    float4 N = normalize(float4(input.normWS, 0.0f));

    LightingResult lit;

    float shadow;

#ifdef DO_CASCADE_SHADOW
    uint layer = 0;
    float dephtVal = abs(input.pos.w);
    bool drawDebug = false;
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        if (dephtVal < ((float[4])(CascData.Distances[i / 4]))[i % 4])
        //if (dephtVal < ((float[4])CascData.Distances[i/4])[i%4])
        {
            layer = i;
            break;
        }
    }

    float4 debugColorCoef = 1.0f;
    if (drawDebug && layer % 4 == 0) debugColorCoef.yz = 0.2;
    if (drawDebug && layer % 4 == 1) debugColorCoef.xz = 0.2;
    if (drawDebug && layer % 4 == 2) debugColorCoef.xy = 0.2;
    if (drawDebug && layer % 4 == 3) debugColorCoef.x = 0.2;
    
    float4 shadowPosH = mul(input.posWS, mul(CascData.ViewProj[layer], mT));
    shadow = CalcCascadeShadowFactor(ShadCompSamp, CascadeShadowMaps, shadowPosH, layer);
    lit = DoLighting(Lights, material, viewData.camPos, input.posWS, N, shadow);
    
    diffuse *= lit.diffuse;
    specular = lit.specular*material.specularScale;
    diffuse *= debugColorCoef;
    specular *= debugColorCoef;
#endif

#ifndef DO_CASCADE_SHADOW
    float4 shadowPosH = mul(input.posWS, mul(lightData.mViewProj, mT));
    shadow = CalcShadowFactor(ShadCompSamp, ShadowMap, shadowPosH);
    lit = DoLighting(Lights, material, viewData.camPos, input.posWS, N, shadow);
    diffuse *= lit.diffuse;
    specular = lit.specular * material.specularScale;
#endif

    float4 col = float4((ambient + emissive + diffuse + specular).rgb, 1.0f);
    col.rgb = pow(col.rgb, 1.0f / 2.2f);

    return col;
}

#endif
