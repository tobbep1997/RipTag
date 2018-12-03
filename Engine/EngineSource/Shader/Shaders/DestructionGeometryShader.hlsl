cbuffer destructionTimer : register(b0)
{
	float4 timer;
}


struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;
};

struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;
};

[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputstream)
{
	GS_OUTPUT output = (GS_OUTPUT)0;

	float3 offsetNormal = cross((input[1].pos - input[0].pos), (input[2].pos - input[0].pos));


	for (uint i = 0; i < 3; i ++)
	{

		output.pos = input[i].pos;
		output.worldPos = input[i].worldPos;
		output.normal = input[i].normal;
		output.TBN = input[i].TBN;
		output.uv = input[i].uv;
		output.color = input[i].color;
		output.info = input[i].info;

		float oo = abs(sin(timer.x));
		output.pos.xyz += normalize(offsetNormal) * oo;
		output.worldPos.xyz += normalize(offsetNormal) * oo;
	

		outputstream.Append(output);
	}

	outputstream.RestartStrip();

//return outputstream;
}