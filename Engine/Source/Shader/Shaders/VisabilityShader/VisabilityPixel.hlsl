#include "../LightCalc.hlsli"
RWTexture2D<uint> OutputMap : register(u1);

Texture2D guardDepthTex : register(t10);
void main (VS_OUTPUT input)
{
    uint w, h;
    guardDepthTex.GetDimensions(w, h);
    float3 fragmentLightPosition = input.pos.xyz / float3(w, h, 1.0f);
    float2 smTex = float2(fragmentLightPosition.x, fragmentLightPosition.y);

    clip(guardDepthTex.Sample(defaultSampler, smTex).r - fragmentLightPosition.z);


    float lightLevel = length(OptimizedLightCalculation(input).rgb);
    InterlockedAdd(OutputMap[int2(0, 0)], (uint) (lightLevel * 100));
}