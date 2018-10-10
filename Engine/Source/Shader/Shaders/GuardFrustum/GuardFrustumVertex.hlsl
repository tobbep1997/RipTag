#include "../StaticConstantBuffers.hlsli"
cbuffer GUARD_BUFFER : register(b5)
{
    float4x4 ViewProj;
    float4x4 ViewProjInverse;
    float4x4 WorldMatrix;
};

struct VS_INPUT
{
    float4 pos : POSITION;
    float4 uv : UV;
    float4 a : ALPHA;
}; 

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORDS;
    float a : ALPHALEVEL;
};

VS_OUT main(VS_INPUT input)
{
    VS_OUT output;
    output.a = input.a.x;
    output.uv = input.uv.xy;
    float4 pos = mul(input.pos, ViewProjInverse);
    pos.xyz /= pos.w;
    pos.w = 1;
    //pos = mul(pos, WorldMatrix);
    output.pos = mul(pos, viewProjection);
    return output;
}