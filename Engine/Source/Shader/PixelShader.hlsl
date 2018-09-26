#pragma warning(disable : 3078)
#pragma warning(disable : 3557)
#pragma warning(disable : 3570)
#include "Shaders/LightCalc.hlsl"

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return OptimizedLightCalculation(input);
    
}