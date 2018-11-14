#include "Shaders/LightCalc.hlsli"


float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 dummy;
	//input.uv.y = 1 - input.uv.y;

	//return diffuseTexture.Sample(defaultSampler, input.uv);
	//return float4(input.normal.xyz,1);
	return OptimizedLightCalculation(input, dummy);
}