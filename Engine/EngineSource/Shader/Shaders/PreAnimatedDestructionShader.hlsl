#include "StaticConstantBuffers.hlsli"

cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

cbuffer SKINNING_BUFFER : register(b4)
{
	row_major float4x4 skinningMatrices[128];
};
cbuffer SKINNING_BUFFER : register(b5)
{
	float4x4 lerpFromTransformMatrix;
	float4 timer;
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


float4x4 createScaleMatrix(float scale)
{
	float4x4 scaleMatrix;

	scaleMatrix._11 = 0;
	scaleMatrix._12 = 0;
	scaleMatrix._13 = 0;
	scaleMatrix._14 = 0;
	
	scaleMatrix._21 = 0;
	scaleMatrix._22 = scale;
	scaleMatrix._23 = 0;
	scaleMatrix._24 = 0;
	
	scaleMatrix._31 = 0;
	scaleMatrix._32 = 0;
	scaleMatrix._33 = 0;
	scaleMatrix._34 = 0;
	
	scaleMatrix._41 = 0;
	scaleMatrix._42 = 0;
	scaleMatrix._43 = 0;
	scaleMatrix._44 = 1;


	return  scaleMatrix;
}

void main(VS_INPUT input, uint vId : SV_VertexID)
{
	float lerpValue = timer.x;
	float4x4 finalWorldPos = lerpFromTransformMatrix;

	float scaleLerpX = 1;
	float scaleLerpY = 0.3;
	float posLerpX = 0;
	float posLerpY = 2;

	float4x4 lerpTowards = lerpFromTransformMatrix;
	float4x4 lerpFrom = lerpFromTransformMatrix;
	float posLerp = 0;

	if (lerpValue <= 0.25)
	{
		posLerp = lerpValue * 4;
	}
	else if (lerpValue > 0.25 && lerpValue <= 0.75)
	{
		posLerp = (lerpValue - 0.25) / 0.5;
		lerpFrom = lerpFromTransformMatrix;
		lerpTowards = worldMatrix;

	}
	else if (lerpValue > 0.75)
	{
		posLerp = (lerpValue - 0.75) * 4;
		lerpFrom = worldMatrix;
		lerpTowards = worldMatrix;
	}


	finalWorldPos = lerp(lerpFrom, lerpTowards, lerpValue);
	
	
	
	



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
	nor = mul(float4(nor, 0.0f), finalWorldPos).xyz;
	//TODO remove
	float3 tangent = normalize(mul(float4(tan, 0.0f), worldMatrix).xyz);
	tangent = normalize(tangent - dot(tangent, nor) * nor).xyz;
	float3 o[3] =
	{
		mul(float4(position, 1.0f), finalWorldPos).xyz,
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