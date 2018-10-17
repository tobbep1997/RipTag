#include "StaticConstantBuffers.hlsli"
cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

float4 main(VS_INPUT input) : SV_POSITION
{
	return mul(input.pos, worldMatrix);	
}