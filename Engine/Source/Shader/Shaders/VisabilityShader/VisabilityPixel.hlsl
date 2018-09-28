#include "../LightCalc.hlsl"

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
    
    //float4 fragmentLightPosition = mul(input.worldPos, viewProjection);
    //        // Get the texture coords of the "object" in the shadow map
    //fragmentLightPosition.xyz /= fragmentLightPosition.w;
    //        // Texcoords are not [-1, 1], change the coords to [0, 1]
    //float2 smTex = float2(0.5f * fragmentLightPosition.x + 0.5f, -0.5f * fragmentLightPosition.y + 0.5f);

    //float depth = fragmentLightPosition.z;

    //if (smTex.x <= 0 || smTex.x >= 1 || depth <= 0.0f || depth > 1.0f)
    //    discard;

    //if (smTex.y <= 0 || smTex.y >= 1 || depth <= 0.0f || depth > 1.0f)
    //    discard;
     

    //bool EXIST = (guardDepthTex.Sample(defaultSampler, smTex).r < depth) ? false : true;
    
    //if (!EXIST)
    //{
    //    discard;
    //}

    //float lightLevel = length(OptimizedLightCalculation(input).rgb);
    //InterlockedAdd(OutputMap[int2(0, 0)], (uint) (lightLevel * 100));

}