cbuffer OBJECT_BUFFER : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
    float4 cameraPosition;
    float4x4 viewProjection;
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
    float3x3 TBN : TBN;
    float2 uv : UV;
};

VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;
	
	output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
    output.worldPos = mul(input.pos, worldMatrix);
	output.normal = normalize(mul(input.normal, worldMatrix));
	float3 tangent = normalize(mul(input.tangent, worldMatrix).xyz);
	tangent = normalize(tangent - dot(tangent, output.normal.xyz)*output.normal.xyz).xyz;
	float3 bitangent = cross(output.normal.xyz, tangent);
	float3x3 TBN = float3x3(tangent, bitangent, output.normal.xyz);
	
	output.TBN = TBN;

    output.uv = input.uv;
	return output;
}