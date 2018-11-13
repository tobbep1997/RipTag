struct VS_OUTPUT
{
    float4 worldPos : SV_POSITION;
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    float4 dummy;
    return float4(1,0,0,1);
}