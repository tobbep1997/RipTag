struct VS_OUTPUT
{
	float4 worldPos : POSITION;
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return float4(1, input.tangent.x, input.tangent.x, .1f);
}