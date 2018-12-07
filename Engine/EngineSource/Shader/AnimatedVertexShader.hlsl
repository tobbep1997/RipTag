#include "Shaders/StaticConstantBuffers.hlsli"

cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

cbuffer OBJECT_BUFFER : register(b7)
{
    int4 usingTexture;
    float4 textureTileMult;
    float4 color;
};

struct VS_INPUT2
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : UV;
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

VS_OUTPUT main(VS_INPUT2 input)
{

	VS_OUTPUT output;
    float4 wp = float4(input.worldPos.xyz,1);

    output.pos = mul(wp, viewProjection);
    output.worldPos = wp;
    output.normal = float4(normalize(input.normal), 0);

    float3 bitangent = cross(output.normal.xyz, input.tangent);
    float3x3 TBN = float3x3(input.tangent, bitangent, output.normal.xyz);
    output.TBN = TBN;
    output.uv = input.uv * textureTileMult.xy;
    output.color = color;
    output.info = int4(1, 0, 0, 0);
	return output;
}