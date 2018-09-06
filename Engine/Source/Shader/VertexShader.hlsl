
struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	return input;
	VS_OUTPUT output;
	output.pos = input.pos;
	output.color = output.color;
	return output;
}