#include "LightCalc.hlsli"

struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;
	float timerValue : TIMER;
};

float4 main(GS_OUTPUT input) : SV_TARGET
{
	VS_OUTPUT inin;
	inin.pos = input.pos;
	inin.worldPos = input.worldPos;
	inin.normal = input.normal;
	inin.TBN = input.TBN;
	inin.uv = input.uv;
	inin.color = input.color;
	inin.info = input.info;

	float4 dummy;
	dummy = OptimizedLightCalculation(inin, dummy);
	float4 colorLerpFirst = dummy;
	float4 colorLerpSecond = float4(0.4, 0.6, 0.5, 1);

	float lerpValue = input.timerValue;

	if (lerpValue > 1)
	{
		lerpValue -= 1;

		colorLerpFirst = float4(0.4, 0.6, 0.5, 1);
		colorLerpSecond = dummy;
	}
	float4 finalColour = lerp(colorLerpFirst, colorLerpSecond, lerpValue);
		

	return finalColour;
}