struct ViewData
{
    matrix mWorld;
    matrix mView;
    matrix mProj;
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
    float4 worldPos : TEXCOORD0;
    float2 tex : TEXCOORD1;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 norm : NORMAL;
    float4 worldPos : TEXCOORD0;
    float2 tex : TEXCOORD1;
};

cbuffer ViewBuf : register(b0)
{
ViewData viewData;
}

cbuffer DirLightBuf : register(b1)
{
DirectionLightData lightData;
}

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    //output.pos = float4(input.pos.xyz + OffsetTransformData.offsetLoc.xyz, 1.0f);
    output.pos = mul(input.pos, viewData.mWorld);
    output.pos = mul(output.pos, viewData.mView);
    output.pos = mul(output.pos, viewData.mProj);
    output.norm = mul(input.norm, viewData.mWorld);
    //output.norm = input.norm;
    output.worldPos = mul(input.pos, viewData.mWorld);
    output.tex = input.tex;
    //output.col = abs(sin(viewData.color));

    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    /*float4 col = 0;
    col += saturate( dot( (float3)lightData.direction, input.norm) * lightData.color );
    col *= DiffuseMap.Sample(Sampler, float2(input.tex.x, 1.0f - input.tex.y));
    col.a = 1.0f;*/
    float4 diffVal = DiffuseMap.Sample(Sampler, float2(input.tex.x, 1.0f - input.tex.y));
    clip(diffVal.a - 0.01f);

    float3 kd = diffVal.xyz;
    float3 normal = normalize(input.norm.xyz);

    //float3 viewDir = normalize(viewData.camPos.xyz - input.worldPos.xyz);
    float3 viewDir = normalize(viewData.camPos.xyz - input.worldPos.xyz);
    float3 lightDir = -lightData.direction.xyz;
    float3 refVec = normalize(reflect(lightDir, normal));

    float3 diffuse = max(0, dot(lightDir, normal)) * kd;
    float3 ambient = kd * lightData.kaSpecPowKsX.x;
    float3 spec = pow(max(0, dot(-viewDir, refVec)), lightData.kaSpecPowKsX.y) * lightData.kaSpecPowKsX.z;

    float4 col = float4(lightData.color.xyz * (diffuse + ambient + spec), 1.0f);
    col.rgb = pow(col.rgb, 1.0f / 2.2f);
    return col;
}
