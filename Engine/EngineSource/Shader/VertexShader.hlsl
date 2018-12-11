#include "Shaders/StaticConstantBuffers.hlsli"


struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 uv : UV;

    float4x4 worldMatrix : WORLD;

	float4 color : COLOR;
	float4 uvMult :UVMULT;
	int4 info : INFO;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;
};

VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;
    input.pos.w = 1.0f;

    output.pos = mul(input.pos, mul(input.worldMatrix, viewProjection));
    output.worldPos = mul(input.pos, input.worldMatrix);
    output.normal = normalize(mul(input.normal, input.worldMatrix));
    float3 tangent = normalize(mul(input.tangent, input.worldMatrix).xyz);
	tangent = normalize(tangent - dot(tangent, output.normal.xyz)*output.normal.xyz).xyz;
    float3 bitangent = cross(tangent, output.normal.xyz);
	float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
	output.TBN = TBN;

	output.color = input.color;
	output.info = input.info;

    output.uv = input.uv.xy * input.uvMult.xy;
	return output;
}