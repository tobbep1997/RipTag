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
    float4 uv : UV;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input)
{

    VS_OUTPUT output =(VS_OUTPUT)0;
    input.pos.w = 1.0f;
    
    output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
    output.worldPos = float4(0, 0, 0, 0);
    output.normal = float4(0, 0, 0, 0);
    output.TBN = (float3x3) 0;
    output.uv = input.uv.xy;

    return output;
}