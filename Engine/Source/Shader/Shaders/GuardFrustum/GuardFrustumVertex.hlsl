cbuffer CAMERA_BUFFER : register(b1)
{
    float4 cameraPosition;
    float4x4 viewProjection;
};
cbuffer GUARD_BUFFER : register(b0)
{
    float4x4 ViewProj;
    float4x4 ViewProjInverse;
    float4x4 WorldMatrix;
};

struct VS_INPUT
{
    float4 pos : POSITION;
}; 

struct VS_OUT
{
    float4 poss : SV_Position;
    float3 col : Color;
};

VS_OUT main(VS_INPUT input)
{
    VS_OUT output;
    output.col = float3(0, 0, 0);
    if (input.pos.x > 0)
        output.col.r = 1;
    else if (input.pos.y > 0)
        output.col.g = 1;
    else if (input.pos.z > 0)
        output.col.b = 1;

    float4 pos = mul(input.pos, ViewProjInverse);
    pos.xyz /= pos.w;
    pos.w = 1;
    pos = mul(pos, WorldMatrix);
    output.poss = mul(pos, viewProjection);
    return output;
}