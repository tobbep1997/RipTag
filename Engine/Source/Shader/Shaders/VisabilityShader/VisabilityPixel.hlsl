#include "../LightCalc.hlsl"

RWTexture2D<uint> OutputMap : register(u0);
void main (VS_OUTPUT input)
{
    float lightLevel = length(OptimizedLightCalculation(input));
    InterlockedAdd(OutputMap[int2(0, 0)], (uint) (lightLevel * 100));
}