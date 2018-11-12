struct VS_INPUT
{
	float4 pos : POSITION;
	float4 tangent : TANGENT;
	float4 normal : NORMAL;
	float2 uv : UV;
};

struct VS_OUTPUT
{
	float4 worldPos : SV_POSITION;
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	output.pos = input.pos;
	output.worldPos = input.pos;// mul(input.pos, worldMatrix);
	output.normal = input.normal;// normalize(mul(input.normal, worldMatrix));
	output.tangent = input.tangent;// mul(input.tangent, worldMatrix);
	output.uv = input.uv;
	return output;
}