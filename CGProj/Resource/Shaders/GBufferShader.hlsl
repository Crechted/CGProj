#ifndef G_BUFFER
#define G_BUFFER

#include "Structures.hlsl"
cbuffer ViewBuf : register(b0)
{
    ViewData viewData;
}

cbuffer MaterialBuf : register(b2)
{
Material material;
}

Texture2D AmbientTex : register( t0 );
Texture2D EmissiveTex : register( t1 );
Texture2D DiffuseTex : register( t2 );
Texture2D SpecularTex : register( t3 );
Texture2D SpecularPowerTex : register( t4 );
Texture2D NormalTex : register( t5 );
Texture2D BumpTex : register( t6 );
Texture2D OpacityTex : register( t7 );

sampler LinearRepeatSampler : register( s0 );

struct GBuffer
{
    float4 LightAccumulation : SV_Target0; // Ambient + emissive (R8G8B8_????) Unused (A8_UNORM)
    float4 Diffuse : SV_Target1;           // Diffuse Albedo (R8G8B8_UNORM) Unused (A8_UNORM)
    float4 Specular : SV_Target2;          // Specular Color (R8G8B8_UNROM) Specular Power(A8_UNORM)
    float4 NormalVS : SV_Target3;          // View space normal (R32G32B32_FLOAT) Unused (A32_FLOAT)
};

float3 ExpandNormal( float3 n )
{
    return n * 2.0f - 1.0f;
}

float4 DoNormalMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv )
{
    float3 normal = tex.Sample( s, uv ).xyz;
    normal = ExpandNormal( normal );

    normal = mul( normal, TBN );
    return normalize( float4( normal, 0 ) );
}

float4 DoBumpMapping( float3x3 TBN, Texture2D tex, sampler s, float2 uv, float bumpScale )
{
    // Sample the heightmap at the current texture coordinate.
    float height_00 = tex.Sample( s, uv ).r * bumpScale;
    // Sample the heightmap in the U texture coordinate direction.
    float height_10 = tex.Sample( s, uv, int2( 1, 0 ) ).r * bumpScale;
    // Sample the heightmap in the V texture coordinate direction.
    float height_01 = tex.Sample( s, uv, int2( 0, 1 ) ).r * bumpScale;

    float3 p_00 = { 0, 0, height_00 };
    float3 p_10 = { 1, 0, height_10 };
    float3 p_01 = { 0, 1, height_01 };

    // normal = tangent x bitangent
    float3 normal = cross( normalize(p_10 - p_00), normalize(p_01 - p_00) );

    // Transform normal from tangent space to view space.
    normal = mul( normal, TBN );

    return float4( normal, 0 );
}

PS_IN VS(VS_IN input)
{
    PS_IN output = (PS_IN)0;

    output.tangWS = mul(float4(input.tang.xyz, 0.0f), viewData.mWorld).xyz;
    output.binormWS = mul(float4(input.binorm.xyz, 0.0f), viewData.mWorld).xyz;
    output.normWS = mul(float4(input.norm.xyz, 0.0f), viewData.mWorld).xyz;
    output.texCoord = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    
    output.posWS = mul(float4(input.pos.xyz, 1.0f), viewData.mWorld);
    output.pos = mul(output.posWS, viewData.mViewProj);
    return output;
}

//[earlydepthstencil]
GBuffer PS(PS_IN input)
{
    GBuffer OUT;

    float4 diffuse = material.diffuseColor;
    if (material.hasDiffuseTex)
    {
        float4 diffuseTex = DiffuseTex.Sample(LinearRepeatSampler, input.texCoord);
        if (any(diffuse.rgb))
        {
            diffuse *= diffuseTex;
        }
        else
        {
            diffuse = diffuseTex;
        }
    }

    float alpha = diffuse.a;
    if (material.hasOpacityTex == 1)
    {
        alpha = OpacityTex.Sample(LinearRepeatSampler, input.texCoord).r;
    }

    if (alpha * material.opacity < material.alphaThreshold)
    {
        discard;
    }

    OUT.Diffuse = diffuse;

    float4 ambient = material.ambientColor;
    if (material.hasAmbientTex == 1)
    {
        float4 ambientTex = AmbientTex.Sample(LinearRepeatSampler, input.texCoord);
        if (any(ambient.rgb))
        {
            ambient *= ambientTex;
        }
        else
        {
            ambient = ambientTex;
        }
    }

    ambient *= material.globalAmbient;

    float4 emissive = material.emissiveColor;
    if (material.hasEmissiveTex == 1)
    {
        float4 emissiveTex = EmissiveTex.Sample(LinearRepeatSampler, input.texCoord);
        if (any(emissive.rgb))
        {
            emissive *= emissiveTex;
        }
        else
        {
            emissive = emissiveTex;
        }
    }

    // TODO: Also compute directional lighting in the LightAccumulation buffer.
    OUT.LightAccumulation = (ambient + emissive);

    float4 N;

    if (material.hasNormalTex == 1)
    {
        // For scense with normal mapping, I don't have to invert the binormal.
        float3x3 TBN = float3x3(normalize(input.tangWS),
            normalize(input.binormWS),
            normalize(input.normWS));

        N = DoNormalMapping(TBN, NormalTex, LinearRepeatSampler, input.texCoord);
    }
    
    else if (material.hasBumpTex == 1)
    {
        // For most scenes using bump mapping, I have to invert the binormal.
        float3x3 TBN = float3x3(normalize(input.tangWS),
            normalize(-input.binormWS),
            normalize(input.normWS));

        N = DoBumpMapping(TBN, BumpTex, LinearRepeatSampler, input.texCoord, material.bumpIntensity);
    }
    else
    {
        N = normalize(float4(input.normWS, 0));
    }

    OUT.NormalVS = N;

    float specularPower = material.specularPower;
    if (material.hasSpecularPowerTex == 1)
    {
        specularPower = SpecularPowerTex.Sample(LinearRepeatSampler, input.texCoord).r * material.specularScale;
    }

    float4 specular = 0;
    if (specularPower > 1.0f) // If specular power is too low, don't use it.
    {
        specular = material.specularColor;
        if (material.hasSpecularTex == 1)
        {
            float4 specularTex = SpecularTex.Sample(LinearRepeatSampler, input.texCoord);
            if (any(specular.rgb))
            {
                specular *= specularTex;
            }
            else
            {
                specular = specularTex;
            }
        }
    }

    // Method of packing specular power from "Deferred Rendering in Killzone 2" presentation 
    // from Michiel van der Leeuw, Guerrilla (2007)
    OUT.Specular = float4(specular.rgb, log2(specularPower) / 10.5f);

    return OUT;
}

#endif
