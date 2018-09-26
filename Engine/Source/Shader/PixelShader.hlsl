#include "Shaders/LightCalc.hlsl"

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return OptimizedLightCalculation(input);
}