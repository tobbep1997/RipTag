#include "Shaders/StaticConstantBuffers.hlsli"

cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

cbuffer SKINNING_BUFFER : register(b2)
{
	row_major float4x4 skinningMatrices[128];
};

//struct VS_INPUT
//{
//	float4 pos : POSITION;
//	float4 normal : NORMAL;
//	float4 tangent : TANGENT;
//	float2 uv : UV;
//	
//};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
	uint4 boneIndices : JOINTINFLUENCES;
	float4 weights : JOINTWEIGHTS;
};

/*struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;
};*/

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;
};

//
//VS_OUTPUT main(VS_INPUT input)
//{
//
//	VS_OUTPUT output;
//
//
//
//	output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
//	output.worldPos = mul(input.pos, worldMatrix);
//	output.normal = normalize(mul(input.normal, worldMatrix));
//    float3 tangent = normalize(mul(input.tangent, worldMatrix).xyz);
//    tangent = normalize(tangent - dot(tangent, output.normal.xyz) * output.normal.xyz).xyz;
//    float3 bitangent = cross(output.normal.xyz, tangent);
//    float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
//    output.TBN = TBN;
//	output.uv = input.uv;
//	return output;
//}

//cbuffer OBJECT_BUFFER : register(b0)
//{
//	float4x4 worldMatrix;
//	float4x4 skinningMatrices[256];
//};
//
//cbuffer CAMERA_BUFFER : register(b1)
//{
//	float4x4 viewProjection;
//};
//
//struct VS_INPUT
//{
//	float4 pos : POSITION;
//	float4 normal : NORMAL;
//	float4 tangent : TANGENT;
//	float4 weights : WEIGHTS;
//	uint4 boneIndices : BONE;
//	float2 uv : UV;
//	
//};
//
//struct VS_OUTPUT
//{
//	float4 pos : SV_POSITION;
//	float4 worldPos : WORLD;
//	float4 normal : NORMAL;
//	float4 tangent : TANGENT;
//	float2 uv : UV;
//};
//
VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;

	//init array
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.weights.x;
	weights[1] = input.weights.y;
	weights[2] = input.weights.z;
	weights[3] = input.weights.w;


	//Blend verts
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 nor = float3(0.0f, 0.0f, 0.0f);
	float3 tan = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < 4; i++)
	{
		if (input.boneIndices[i] >= 0) //unused bone indices are negative
		{
			//TODO: cbuffer gWorld
			position += weights[i] * mul(float4(input.pos.xyz, 1.0f), skinningMatrices[input.boneIndices[i] - 1]).xyz;
			nor += weights[i] * mul(input.normal,
				skinningMatrices[input.boneIndices[i] - 1]).xyz;
			tan += weights[i] * mul(input.tangent,
				skinningMatrices[input.boneIndices[i] - 1]).xyz;
		}
	}

	nor = normalize(nor);
	tan = normalize(tan);

	output.pos = mul(float4(position, 1.0f), mul(worldMatrix, viewProjection));
	output.worldPos = mul(float4(position, 1.0f), worldMatrix);
	output.normal = mul(float4(nor, 0.0f), worldMatrix);
	float3 tangent = normalize(mul(float4(tan, 0.0f), worldMatrix).xyz);
    tangent = normalize(tangent - dot(tangent, output.normal.xyz) * output.normal.xyz).xyz;
    float3 bitangent = cross(output.normal.xyz, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
    output.TBN = TBN;
	output.uv = input.uv;
	return output;
}