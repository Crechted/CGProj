#ifndef STRUCTURES
#define STRUCTURES

struct ViewData
{
    matrix mWorld;
    matrix mViewProj;
    float4 objectPos;
    float4 camPos;
};

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct LightData
{
    float4 posWS;
    float4 directionWS;
    float4 posVS;
    float4 directionVS;
    float4 color;

    float spotlightAngle;
    float range;
    bool enabled;
    bool selected;

    uint type;
    float3 padding;
};

struct DirectionLightData
{
    float4 direction;
    float4 color;
    float4 kaSpecPowKsX;
    matrix mShadowTransform;
};

struct Material
{
    float4 globalAmbient;
    float4 ambientColor;
    float4 emissiveColor;
    float4 diffuseColor;
    float4 specularColor;
    float4 reflectance;

    float opacity;
    float specularPower;
    float indexofRefraction;
    bool hasAmbientTex;
    
    bool hasEmissiveTex;
    bool hasDiffuseTex;
    bool hasSpecularTex;
    bool hasSpecularPowerTex;

    bool hasNormalTex;
    bool hasBumpTex;
    bool hasOpacityTex;
    float bumpIntensity;

    float specularScale;
    float alphaThreshold;
    float2 padding;
};

#ifdef DO_CASCADE_SHADOW
struct CascadeData
{
    matrix ViewProj[CASCADE_COUNT];
    float4 Distances[CASCADE_COUNT/4];
};
#endif

struct VS_IN
{
    float3 pos : POSITION;
    float3 tang : TANGENT;
    float3 binorm : BINORMAL;
    float3 norm : NORMAL;
    float2 texCoord : TEXCOORD0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float3 tangVS : TANGENT;
    float3 binormVS : BINORMAL;
    float3 normVS : NORMAL;
    float4 posWS : TEXCOORD1;
    float4 ShadowPosH : TEXCOORD2;
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

#endif
