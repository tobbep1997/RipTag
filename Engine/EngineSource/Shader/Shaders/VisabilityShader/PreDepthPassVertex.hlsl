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
    float4 uv : UV;

    float4x4 worldMatrix : WORLD;

    float4 color : COLOR;
    float4 uvMult : UVMULT;
    int4 info : INFO;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{

    VS_OUTPUT output;
    
    output.pos = mul(input.pos, mul(input.worldMatrix, viewProjection));
    //output.pos = mul(output.pos, viewProjection);
   
    return output;
}