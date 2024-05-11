#ifndef TRIANGLE_SHADER
#define TRIANGLE_SHADER

//#define DO_CASCADE_SHADOW

#include "LightingFunctions.hlsl"
#include "Structures.hlsl"

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
DirectionLightData lightData;
}

#ifdef DO_CASCADE_SHADOW
cbuffer CascadeBuf : register(b2)
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

    output.tangVS = mul(float4(input.tang.xyz, 0.0f), viewData.mWorld).xyz;
    output.binormVS = mul(float4(input.binorm.xyz, 0.0f), viewData.mWorld).xyz;
    output.normVS = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld).xyz;
    output.tex = input.texCoord;
    float4 pos = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);

    //pos += sin(pos.x * 10.0f * lightData.kaSpecPowKsX.w);
    output.posWS = pos;
    output.pos = pos;
    output.pos = mul(output.pos, viewData.mViewProj);

    output.ShadowPosH = mul(output.posWS, lightData.mShadowTransform);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 diffVal = DiffuseTex.Sample(DiffSamp, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);
    if (lightData.color.r == float4(0.0f, 0.0f, 0.0f, 0.0f).r) return diffVal;

    float3 kd = diffVal.xyz;
    float3 normal = normalize(input.normVS.xyz);

    float3 viewDir = normalize(viewData.camPos.xyz - input.posWS.xyz);
    float3 lightDir = -lightData.direction.xyz;
    float3 refVec = normalize(reflect(lightDir, normal));

    float3 diffuse = max(0, dot(lightDir, normal)) * kd;
    float3 ambient = kd * lightData.kaSpecPowKsX.x;
    float3 spec = pow(max(0, dot(-viewDir, refVec)), lightData.kaSpecPowKsX.y) * lightData.kaSpecPowKsX.z;

    float shadow;
    float4 lightColor = lightData.color;

#ifdef DO_CASCADE_SHADOW
    uint layer = 0;
    float dephtVal = abs(input.pos.w);
    bool drawDebug = false;
    for (int i = 0; i < CASCADE_COUNT; ++i)
    {
        if (dephtVal < ((float[4])(CascData.Distances[i/4]))[i%4])
            //if (dephtVal < ((float[4])CascData.Distances[i/4])[i%4])
            {
            layer = i;
            break;
            }
    }  

    if (drawDebug && layer%4 == 0)  lightColor.yz = 0.2;
    if (drawDebug && layer%4 == 1)  lightColor.xz = 0.2;
    if (drawDebug && layer%4 == 2)  lightColor.xy = 0.2;
    if (drawDebug && layer%4 == 3)  lightColor.x = 0.2;
    matrix mT = {
        {0.5f, 0.0f, 0.0f, 0.0f},
        {0.0f, -0.5f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}};
    input.ShadowPosH = mul(input.posWS  , mul(CascData.ViewProj[layer], mT));
    shadow = CalcCascadeShadowFactor(ShadCompSamp, CascadeShadowMaps, input.ShadowPosH, layer);
#endif

#ifndef DO_CASCADE_SHADOW
    shadow = CalcShadowFactor(ShadCompSamp, ShadowMap, input.ShadowPosH);
#endif
    diffuse *= shadow;
    spec *= shadow;
    float4 col = float4(lightColor.xyz * (diffuse + ambient + spec), 1.0f);
    col.rgb = pow(col.rgb, 1.0f / 2.2f);

    return col;
}

#endif
