#include "Shaders/StaticConstantBuffers.hlsli"

cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

cbuffer SKINNING_BUFFER : register(b4)
{
	row_major float4x4 skinningMatrices[128];
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
	uint4 boneIndices : JOINTINFLUENCES;
	float4 weights : JOINTWEIGHTS;
};

RWBuffer<float> outputUAV : register(u0);

void main(VS_INPUT input, uint vId : SV_VertexID)
{

	

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
    nor = mul(float4(nor, 0.0f), worldMatrix).xyz;
    //TODO remove
	float3 tangent = normalize(mul(float4(tan, 0.0f), worldMatrix).xyz);
    tangent = normalize(tangent - dot(tangent, nor) * nor).xyz;
    float3 o[3] = 
    {
        mul(float4(position, 1.0f), worldMatrix).xyz,
        nor,
        tangent
    };

	//= mul(float4(position, 1.0f), mul(worldMatrix, viewProjection)).xyz;
    
    const uint dat = vId * 11;
    

    outputUAV[dat + 0] = o[0].x;
    outputUAV[dat + 1] = o[0].y;
    outputUAV[dat + 2] = o[0].z;
    
    outputUAV[dat + 3] = o[1].x;
    outputUAV[dat + 4] = o[1].y;
    outputUAV[dat + 5] = o[1].z;

    outputUAV[dat + 6] = o[2].x;
    outputUAV[dat + 7] = o[2].y;
    outputUAV[dat + 8] = o[2].z;

    outputUAV[dat + 9] = input.uv.x;
    outputUAV[dat + 10] = input.uv.y;

    
    
    //        outputUAV[dat + 0] = 5;
    //output.worldPos.x;
    //outputUAV[ dat + 1] = 6;
    //output.worldPos.y;
    //outputUAV[ dat + 2] = 7;
    //output.worldPos.z;

    //outputUAV[ dat + 3] = 8;
    //output.normal.x;
    //outputUAV[ dat + 4] = 9;
    //output.normal.y;
    //outputUAV[ dat + 5] = 10;
    //output.normal.z;

    //outputUAV[ dat + 6] = 11;
    //tangent.x;
    //outputUAV[ dat + 7] = 12;
    //tangent.y;
    //outputUAV[ dat + 8] = 13;
    //tangent.z;


	//return output;
}