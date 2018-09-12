//SamplerComparisonState sampState : register(s0);


struct GSOutput
{
	float4 pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUTPUT
{
	float4 pos0 : SV_TARGET0;
	float4 pos1 : SV_TARGET1;
	float4 pos2 : SV_TARGET2;
	float4 pos3 : SV_TARGET3;
	float4 pos4 : SV_TARGET4;
	float4 pos5 : SV_TARGET5;
};

PS_OUTPUT main(GSOutput input) : SV_TARGET
{	
	PS_OUTPUT output;

output.pos0 = float4(0, 0,0, 1);
output.pos1 = float4(0, 0,0, 1);
output.pos2 = float4(0, 0,0, 1);
output.pos3 = float4(0, 0,0, 1);
output.pos4 = float4(0, 0,0, 1);
output.pos5 = float4(0, 0,0, 1);

	if (input.RTIndex == 0)
		output.pos0 = input.pos;
	
	if (input.RTIndex == 1)
		output.pos1 = input.pos;
	
	if (input.RTIndex == 2)
		output.pos2 = input.pos;
	
	if (input.RTIndex == 3)
		output.pos3 = input.pos;
	
	if (input.RTIndex == 4)
		output.pos4 = input.pos;
	
	if (input.RTIndex == 5)
		output.pos5 = input.pos;
	return output;
	
}