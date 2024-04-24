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
    //float4 worldPos : TEXCOORD0;
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

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosH)
{
    // perspective division
    shadowPosH.xyz /= shadowPosH.w;

    // Depth value of NDC space
    float depth = shadowPosH.z;

    // The width and height of the texel in texture coordinates
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
}

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
DirectionLightData lightData;
}

Texture2D DiffuseMap : register(t0);
SamplerState DiffSamp : register(s0);
Texture2D ShadowMap : register(t1);
//SamplerState ShadSamp : register(s1);
SamplerComparisonState ShadCompSamp : register(s1);

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.norm = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld);
    output.tex = input.tex;
    float4 pos = mul(input.pos, viewData.mWorld);
    pos += output.norm * sin(pos.x * 10.0f * lightData.kaSpecPowKsX.w);

    output.worldPos = pos;
    output.pos = pos;
    output.pos = mul(output.pos, viewData.mView);
    output.pos = mul(output.pos, viewData.mProj);

    output.ShadowPosH = mul(output.worldPos, viewData.mShadowTransform);
    return output;
}

float4 PS(PS_IN input) : SV_Target
{

    float4 diffVal = DiffuseMap.Sample(DiffSamp, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);

    float3 kd = diffVal.xyz;
    float3 normal = normalize(input.norm.xyz);

    //float3 viewDir = normalize(viewData.camPos.xyz - input.worldPos.xyz);
    float3 viewDir = normalize(viewData.camPos.xyz - input.worldPos.xyz);
    float3 lightDir = -lightData.direction.xyz;
    //float3 lightDir = lightData.direction.xyz-input.worldPos.xyz;
    float3 refVec = normalize(reflect(lightDir, normal));

    float3 diffuse = max(0, dot(lightDir, normal)) * kd;
    float3 ambient = kd * lightData.kaSpecPowKsX.x;
    float3 spec = pow(max(0, dot(-viewDir, refVec)), lightData.kaSpecPowKsX.y) * lightData.kaSpecPowKsX.z;

    // Sample texture by projecting texture coordinates
    // The sampled r component is the depth value when the light source observes the point
    
    float shadow[5] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    shadow[0] = CalcShadowFactor(ShadCompSamp, ShadowMap, input.ShadowPosH);
    diffuse *= shadow[0];
    spec *= shadow[0];
    
    float4 col;

    if (lightData.color.r == float3(0.0f, 0.0f, 0.0f).r)
    {
        col = diffVal;
    }
    else
    {
        col = float4(lightData.color.xyz * (diffuse + ambient + spec), 1.0f);
    }

    col.rgb = pow(col.rgb, 1.0f / 2.2f);
    return col;
}
