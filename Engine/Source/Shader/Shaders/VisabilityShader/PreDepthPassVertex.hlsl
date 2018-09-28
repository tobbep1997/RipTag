cbuffer OBJECT_BUFFER : register(b0)
{
    float4x4 worldMatrix;
};

cbuffer GUARD_VIEW : register(b1)
{
    float4 cameraPosition;
    float4x4 viewProj;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : UV;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{

    VS_OUTPUT output;
    
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewProj);
   
    return output;
}