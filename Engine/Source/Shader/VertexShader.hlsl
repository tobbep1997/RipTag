cbuffer EXAMPLE_BUFFER : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 worldMatrix;
};


struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;
	

	output.pos = mul(input.pos, mul(worldMatrix, mul(view,  projection)));
	output.color = input.color;
	return output;
}