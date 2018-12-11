#include "../Shaders/StaticConstantBuffers.hlsli"


struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 uv : UV;

    float4x4 worldMatrix : WORLD;

	float4 color : COLOR;
	float4 uvMult :UVMULT;
	int4 info : INFO;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;

    float TessFactor : TESS;
};
#define MAX_TESS 64
#define MIN_TESS 1
#define MIN_TESS_DIST 5
#define MAX_TESS_DIST 20
VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos;// = mul(input.pos, mul(input.worldMatrix, viewProjection));
    output.worldPos = mul(input.pos, input.worldMatrix);
    output.normal = normalize(mul(input.normal, input.worldMatrix));
    float3 tangent = normalize(mul(input.tangent, input.worldMatrix).xyz);
	tangent = normalize(tangent - dot(tangent, output.normal.xyz)*output.normal.xyz).xyz;
	float3 bitangent = cross(output.normal.xyz, tangent);
	float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
	output.TBN = TBN;

	output.color = input.color;
	output.info = input.info;

    output.uv = input.uv.xy * input.uvMult.xy;

    float d = distance(cameraPosition, output.worldPos);


    float tess = saturate((MIN_TESS_DIST - d) / (MIN_TESS_DIST - MAX_TESS_DIST));
    output.TessFactor = tess * (MAX_TESS - MIN_TESS);
    output.TessFactor = MAX_TESS - output.TessFactor;

	return output;
}