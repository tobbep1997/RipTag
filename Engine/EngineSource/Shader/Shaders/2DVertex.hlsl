cbuffer OBJECT_BUFFER : register(b3)
{
	float2 pos;
	float2 size;
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
	output.pos = float4((input.pos.x - 1.0f) + (pos.y * 2.0f), (input.pos.y - 1.0f) + (pos.y * 2.0f),0,1);
	output.UV = input.UV;
	return output;
}