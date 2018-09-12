cbuffer OBJECT_BUFFER : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float4x4 viewProjection;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
	uint4 jointinfluences : JOINTINFLUENCES;
	float4 jointweights : JOINTWEIGHTS;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;


	output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
	output.worldPos = mul(input.pos, worldMatrix);
	output.normal = mul(input.normal, worldMatrix);
	output.tangent = mul(input.tangent, worldMatrix);
	output.uv = input.uv;
	return output;
}