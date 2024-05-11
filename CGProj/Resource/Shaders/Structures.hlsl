#ifndef STRUCTURES
#define STRUCTURES

static matrix mT = {
    {0.5f, 0.0f, 0.0f, 0.0f},
    {0.0f, -0.5f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.0f, 1.0f}};

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
    matrix mViewProj;
    
    float4 posWS;
    float4 directionWS;
    float4 posVS;
    float4 directionVS;
    float4 color;

    float spotlightAngle;
    float range;
    bool enabled;
    uint type;
};

/*struct DirectionLightData
{
    float4 direction;
    float4 color;
    float4 kaSpecPowKsX;
    matrix mShadowTransform;
};*/

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
    float indexOfRefraction;
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
    float4 posWS : TEXCOORD1;
    //float3 posWS : TEXCOORD2;
    float3 tangWS : TANGENT;
    float3 binormWS : BINORMAL;
    float3 normWS : NORMAL;
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
