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
	//return float4(0.5f,0.5f,0.5f, 1);
	float4 color = diffuseTexture.Sample(defaultSampler, input.uv);
	float temp = color.w;
	if (input.tangent.x > 0.25f)
	{
		temp -= 0.1f;
		color = MRATexture.Sample(defaultSampler, input.uv);
		color.x -= input.tangent.x - 0.2f;
		color.y -= input.tangent.x - 0.2f;
		color.z -= input.tangent.x - 0.2f;
	}

    return float4(color.xyz, temp);
}