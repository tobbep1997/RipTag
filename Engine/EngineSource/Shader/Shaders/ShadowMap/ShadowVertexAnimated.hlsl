#include "../StaticConstantBuffers.hlsli"

cbuffer OBJECT_BUFFER : register(b3)
{
    float4x4 worldMatrix;
};
struct VS_INPUT2
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : UV;
};

float4 main(VS_INPUT2 input) : SV_POSITION
{
    return float4(input.worldPos, 1.0f);
}