#include "Shaders/LightCalc.hlsli"


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 dummy;
	return OptimizedLightCalculation(input, dummy);
}