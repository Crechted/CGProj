#ifndef STRUCTURES
#define STRUCTURES

struct ViewData
{
    matrix mWorld;
    matrix mView;
    matrix mProj;
    matrix mShadowTransform;
    float4 objectPos;
    float4 camPos;
};

struct DirectionLightData
{
    float4 direction;
    float4 color;
    float4 kaSpecPowKsX;
};

struct VS_IN
{
    float4 pos : POSITION;
    float4 norm : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL;
    float4 worldPos : TEXCOORD1;
    float4 ShadowPosH : TEXCOORD2;
    float2 tex : TEXCOORD0;
};

#endif
