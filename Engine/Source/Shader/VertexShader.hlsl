cbuffer EXAMPLE_BUFFER : register(b0)
{
	float4x4 view;
	float4x4 projection;
	float4x4 worldMatrix;
};


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
    float4 worldPos : WORLD;
	float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;
	

	output.pos = mul(input.pos, mul(worldMatrix, mul(view,  projection)));
    output.worldPos = mul(input.pos, worldMatrix);
    output.normal = mul(input.normal, worldMatrix);
    output.tangent = mul(input.tangent, worldMatrix);
    output.uv = input.uv;
	return output;
}