cbuffer LIGHT_MATRIX : register(b0)
{
	float4x4 lightViewProjection[6];
};


struct GSOutput
{
	float4 pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(6*3)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	for (uint i = 0; i < 6; i++)
	{
		GSOutput element;
		element.RTIndex = i;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			element.pos = mul(input[vertex], lightViewProjection[i]);
			output.Append(element);
		}
		output.RestartStrip();
	}
}