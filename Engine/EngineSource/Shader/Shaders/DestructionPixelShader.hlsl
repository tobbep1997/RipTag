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
	return float4(1,1,1,1);
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

	float4 colorLerpSecond = dummy;

	float2 scaleFactor = float2(3, 3);

	float2 c = input.uv * scaleFactor - scaleFactor * 0.5f;
	float lerpValue = input.timerValue;

	float v = 0;
	v += sin((c.x + lerpValue));
	v += sin((c.y + lerpValue) * 0.5f);
	v += sin((c.x + c.y + lerpValue) * 0.5f);
	c += scaleFactor * 0.5 * float2(sin(lerpValue* 0.333333f), cos(lerpValue * 0.5f));
	v += sin(sqrt(c.x*c.x + c.y*c.y + 1.0f) + lerpValue);

	v *= 0.5f;

	//colorLerpSecond.xyz = float3(tan(3.141592 * v), sin(3.141592 * v) , cos(3.141592 * v));
	colorLerpSecond.xyz = float3(sin(3.141592 * v), 0.4f , cos(3.141592 * v));
	colorLerpSecond = float4(colorLerpSecond.xyz * 0.5f + 0.5f, 1);
	colorLerpSecond.xyz *= 0.4f;

	if (lerpValue <= 0.5)
	{
		lerpValue = lerpValue * 2;
	}
	else
	{
		//lerpValue = 1 - (lerpValue - 0.5f) / 1.5f;
		lerpValue = 1 - (lerpValue - 0.5f) * 2;
	}
	float4 finalColour = lerp(colorLerpFirst, colorLerpSecond, lerpValue);
		

	return finalColour;
}