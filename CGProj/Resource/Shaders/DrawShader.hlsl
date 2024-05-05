struct ViewData
{
    matrix mWorld;
    matrix mViewProj;
    float4 objectPos;
    float4 camPos;
};

struct VS_IN
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

cbuffer ViewBuf : register(b0)
{
    ViewData viewData;
}

Texture2D DiffuseMap : register(t0);
SamplerState Sampler : register(s0);


PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.pos = mul(input.pos, viewData.mWorld);
    output.pos = mul(output.pos, viewData.mViewProj);
    output.col = input.col;

    return output;
}

float4 PS(PS_IN input) : SV_Target
{
    float4 col = input.col;
    return col;
}
