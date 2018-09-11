struct GSOutput
{
	float4 pos : SV_POSITION;
};

cbuffer LIGHT_PROJECTION :register(b0)
{
	float4x4 viewProjection[6];
};

[maxvertexcount(3)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output0,
	inout TriangleStream< GSOutput > output1,
	inout TriangleStream< GSOutput > output2,
	inout TriangleStream< GSOutput > output3,
	inout TriangleStream< GSOutput > output4,
	inout TriangleStream< GSOutput > output5
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = mul(input[i], viewProjection[i]);
		output0.Append(element);
		output1.Append(element);
		output2.Append(element);
		output3.Append(element);
		output4.Append(element);
		output5.Append(element);
	}
}