struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORDS;
    float a : ALPHALEVEL;
};

float4 main(VS_OUT input) : SV_TARGET
{
    return float4(0, 1, 1, input.a);
}