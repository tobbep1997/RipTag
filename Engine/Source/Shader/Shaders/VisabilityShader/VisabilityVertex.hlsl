#include "../StaticConstantBuffers.hlsli"
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
    float4 worldPos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.worldPos = mul(input.pos, worldMatrix);
    return output;
}