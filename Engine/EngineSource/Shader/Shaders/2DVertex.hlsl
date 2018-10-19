cbuffer OBJECT_BUFFER : register(b3)
{
	float4x4 worldMatrix;
};

struct VS_INPUT
{
	float2 pos : POSITION;
	float2 UV : UV;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 UV : UV;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = float4(input.pos.x, input.pos.y, 0, 1);
	output.UV = input.UV;
	return output;
}