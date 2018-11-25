struct VS_OUTPUT
{
	float4 worldPos : POSITION;
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

SamplerState defaultSampler : register(s1);

Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 color = diffuseTexture.Sample(defaultSampler, input.uv);
	float temp = color.w;

	if(input.tangent.y == 1)
	{
		color = MRATexture.Sample(defaultSampler, input.uv);
		if (input.tangent.x < 0.55f)
		{
			temp = color.w * (1.5 * input.tangent.x);
		}
	}
	else
	{
		if (input.tangent.x < 0.55f)
		{
			color = MRATexture.Sample(defaultSampler, input.uv);
			temp = color.w * (1 * input.tangent.x);
		}
	}
    return float4(color.xyz, temp);
}