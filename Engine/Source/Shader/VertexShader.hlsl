cbuffer EXAMPLE_BUFFER : register(b0)
{
	float val1;
	float val2;
	float val3;
	float val4;
};


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

	VS_OUTPUT output;

	input.pos.x += sin(val1);
	input.color.x += sin(val1);
	input.color.y += sin(val1 - 0.5);
	input.color.z += sin(val1 - 1.5);
	output.pos = input.pos;
	output.color = input.color;
	return output;
}