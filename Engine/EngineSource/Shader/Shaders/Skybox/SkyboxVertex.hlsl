
cbuffer CAMERA_BUFFER : register(b6)
{
    float4x4 skyBoxViewProjection;
};

struct VS_INPUT
{
    float4 position : POSITION;
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
    output.WVP.z /= output.WVP.w;
    output.uv = input.uv.xy;

    return output;
}