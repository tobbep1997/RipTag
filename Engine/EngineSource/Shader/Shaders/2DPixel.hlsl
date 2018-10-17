SamplerState defaultSampler : register(s1);
Texture2D diffuseTexture : register(t1);


struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 UV : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return diffuseTexture.Sample(defaultSampler, input.UV);
}