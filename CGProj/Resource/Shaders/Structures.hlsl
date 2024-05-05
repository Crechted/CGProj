#ifndef STRUCTURES
#define STRUCTURES

struct ViewData
{
    matrix mWorld;
    matrix mViewProj;
    float4 objectPos;
    float4 camPos;
};

struct DirectionLightData
{
    float4 direction;
    float4 color;
    float4 kaSpecPowKsX;
    matrix mShadowTransform;
};

#ifdef DO_CASCADE_SHADOW
struct CascadeData
{
    matrix ViewProj[4];
    float4 Distances;
};
#endif


struct VS_IN
{
    float4 pos : POSITION;
    float4 norm : NORMAL;
    float2 tex : TEXCOORD0;
};

struct GS_IN
{
    float4 pos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
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
