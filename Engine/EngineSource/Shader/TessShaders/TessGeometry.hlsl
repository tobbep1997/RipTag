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

struct DS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;

    float4 color : COLOR;
    int4 info : INFO;
};

/*
[maxvertexcount(3)]
void main(
	triangle DS_OUTPUT input[3],
	inout LineStream<GS_OUTPUT> output
)

*/

[maxvertexcount(3)]
void main(
	triangle DS_OUTPUT input[3],
	inout TriangleStream<GS_OUTPUT> output
)
{
	for (uint i = 0; i < 3; i++)
	{
        GS_OUTPUT element = (GS_OUTPUT)0;
        element.pos         = input[i].pos;
        element.worldPos    = input[i].worldPos;
        element.normal      = input[i].normal;
        element.TBN         = input[i].TBN;
        element.uv          = input[i].uv;
        element.color       = input[i].color;
        element.info        = input[i].info;
		output.Append(element);
	}
}