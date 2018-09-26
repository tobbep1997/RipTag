#pragma warning(disable : 3078)
#pragma warning(disable : 3570)
//#pragma error(disable : 3662)
#include "../LightCalc.hlsl"

RWTexture2D<uint> OutputMap : register(u0);
void main (VS_OUTPUT input)
{
    //InterlockedAdd(OutputMap[int2(0, 0)], 1);
    //InterlockedAdd(OutputMap[int2(1, 0)], 1);
    float4 lol = OptimizedLightCalculation(input);
    
    float lightLevel = length(lol.rgb);
    
    // Per fragment can be lit from 0 to 10
    //return float4(1, 1, 1, 1);
    
    InterlockedAdd(OutputMap[int2(0, 0)], (uint) (lightLevel * 10));
}