#include "Shaders/LightCalc.hlsli"

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return OptimizedLightCalculation(input);
}