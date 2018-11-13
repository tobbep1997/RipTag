#include "../../EngineSource/Shader/Shaders/StaticConstantBuffers.hlsli"

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

struct VS_OUTPUT
{
    float4 worldPos : POSITION;
    float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input) 
{
	VS_OUTPUT output;

    output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
    output.worldPos = mul(input.pos, worldMatrix); // mul(input.pos, worldMatrix);
	output.normal = input.normal;// normalize(mul(input.normal, worldMatrix));
	output.tangent = input.tangent;// mul(input.tangent, worldMatrix);
	output.uv = input.uv;


    



	return output;
}