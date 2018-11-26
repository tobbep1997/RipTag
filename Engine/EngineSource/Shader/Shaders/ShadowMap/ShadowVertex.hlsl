#include "../StaticConstantBuffers.hlsli"

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;

	float4x4 worldMatrix : WORLD;

	float4 color : COLOR;
	float2 uvMult :UVMULT;
	int4 info : INFO;
};

float4 main(VS_INPUT input) : SV_POSITION
{
	return mul(input.pos, input.worldMatrix);
}