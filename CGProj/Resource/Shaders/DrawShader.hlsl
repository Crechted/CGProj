struct ViewData
{
    matrix mWorld;
    matrix mView;
    matrix mProj;
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


PS_IN VSMain(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    //output.pos = float4(input.pos.xyz + OffsetTransformData.offsetLoc.xyz, 1.0f);
    output.pos = mul(input.pos, viewData.mWorld);
    output.pos = mul(output.pos, viewData.mView);
    output.pos = mul(output.pos, viewData.mProj);
    output.col = input.col;
    //output.col = abs(sin(viewData.color));

    return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
    float4 col = input.col;
    return col;
}
