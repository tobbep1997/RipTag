#include "LightCalc.hlsli"
Texture2D depth : register(t10);

cbuffer LIGHTS : register(b8)
{
    float4 outlineColor;
}
struct VS_OUTP
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;
};

float4 main(VS_OUTP input) : SV_TARGET
{

    uint w, h;
    depth.GetDimensions(w, h);
    float3 fragmentLightPosition = input.pos.xyz / float3(w, h, 1.0f);
    float2 smTex = float2(fragmentLightPosition.x, fragmentLightPosition.y);
    //if ((depth.Sample(defaultSampler, smTex).r - fragmentLightPosition.z)  < 0.002f)
    //{
    //    discard;
    //}
   
    return outlineColor;
}