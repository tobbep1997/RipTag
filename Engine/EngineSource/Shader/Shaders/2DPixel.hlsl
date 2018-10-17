//SamplerState defaultSampler : register(s4);
Texture2D diffuseTexture : register(t1);


struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 UV : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return float4(1,0,1,1);
	//return diffuseTexture.Sample(defaultSampler, input.UV);
}