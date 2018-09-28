struct VS_INPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : UV;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{

    VS_OUTPUT output;
	

    output.pos = input.pos;
    return output;
}