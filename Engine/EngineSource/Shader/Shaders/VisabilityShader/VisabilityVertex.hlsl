#include "../StaticConstantBuffers.hlsli"


struct VS_INPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 uv : UV;

    float4x4 worldMatrix : WORLD;

    float4 color : COLOR;
    float4 uvMult : UVMULT;
    int4 info : INFO;
};

struct VS_OUTPUT
{
    float4 worldPos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.worldPos = mul(input.pos, input.worldMatrix);
    return output;
}