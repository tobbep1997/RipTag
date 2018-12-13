#include "SkyboxVertex.hlsl"

SamplerState defaultSampler : register(s1);
Texture2D diffuseTexture : register(t1);

float4 main(VS_OUTPUT input) : SV_Target
{
    input.uv.y = 1.0f - input.uv.y;
    return diffuseTexture.Sample(defaultSampler, input.uv);   
}