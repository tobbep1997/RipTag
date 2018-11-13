#include "LightCalc.hlsli"

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return float4(1.0f, 20.0f / 255.0f, 147.0f / 255.0f, 1.0f);
}