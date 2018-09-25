cbuffer CAMERA_BUFFER : register(b2)
{
    float4 cameraPosition;
    float4x4 viewProjection;
};

struct VS_OUT
{
    float4 poss : SV_Position;
    float3 col : Color;
};

float4 main(VS_OUT input) : SV_TARGET
{
    return float4(input.col, 0.2f);
}