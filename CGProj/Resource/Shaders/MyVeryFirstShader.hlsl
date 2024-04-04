/*struct ConstantData
{
    float4 offset;
    float4 color;
};

cbuffer ConstBuf : register(b0)
{
    ConstantData ConstData;
}*/

struct ViewData
{
    matrix mWorld;
    matrix mView;
    matrix mProj;
};

cbuffer ViewBuf : register(b0)
{
    ViewData viewData;
}

struct VS_IN
{
    float4 pos : POSITION0;
    float4 col : COLOR0;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

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
#ifdef TEST
    //if (input.pos.x > 400) col = TCOLOR;
#endif
    return col;
}
