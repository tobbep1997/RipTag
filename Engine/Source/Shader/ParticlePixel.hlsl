struct VS_OUTPUT
{
	float4 worldPos : SV_POSITION;
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET1
{
	return float4(0.5f, 0.5f, 0.5f, 0);
}