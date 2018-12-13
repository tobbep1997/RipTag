
cbuffer CAMERA_BUFFER : register(b6)
{
    float4x4 skyBoxViewProjection;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 uv : UV;
};

struct VS_OUTPUT
{
    float4 WVP  : SV_POSITION;
    float2 uv   : UV;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.WVP = mul(input.position, skyBoxViewProjection);
    output.WVP.z = output.WVP.w - 0.0000001f;
    output.uv = input.uv.xy;

    return output;
}