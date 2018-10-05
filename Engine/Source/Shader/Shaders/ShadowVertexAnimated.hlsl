#include "StaticConstantBuffers.hlsli"

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

float4 main(VS_INPUT input) : SV_POSITION
{
	//init array
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weights.x;
    weights[1] = input.weights.y;
    weights[2] = input.weights.z;
    weights[3] = input.weights.w;


	//Blend verts
    float3 position = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        if (input.boneIndices[i] >= 0) //unused bone indices are negative
        {
            position += weights[i] * mul(float4(input.pos.xyz, 1.0f), skinningMatrices[input.boneIndices[i] - 1]).xyz;
        }
    }

    return mul(float4(position, 1.0f), worldMatrix);
}