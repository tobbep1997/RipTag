struct VS_OUTPUT
{
	float4 worldPos : POSITION;
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

cbuffer PARTICLE_MODIFIERS : register(b0)
{
	vector <float, 4> alphaMultipliers;
	vector <float, 4> fadePoints;
	vector <float, 4> colorModifiers;
};

SamplerState defaultSampler : register(s1);

Texture2D BeginTexture : register(t1);
Texture2D IntermidiateTexture : register(t2);
Texture2D EndTexture : register(t3);

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float lifeLeft = input.tangent.x;

	if (lifeLeft < fadePoints[0] && lifeLeft > fadePoints[1] && lifeLeft > fadePoints[2])
	{
		color = BeginTexture.Sample(defaultSampler, input.uv);
		color.w *= alphaMultipliers[0] * lifeLeft;
		color *= colorModifiers[0];
		saturate(color);
	}
	else if (lifeLeft < fadePoints[1] && lifeLeft> fadePoints[2])
	{
		color = IntermidiateTexture.Sample(defaultSampler, input.uv);
		color.w *= alphaMultipliers[1] * lifeLeft;
		color *= colorModifiers[1];
		saturate(color);
	}
	else if (lifeLeft < fadePoints[2] && lifeLeft > 0.01f)
	{
		color = EndTexture.Sample(defaultSampler, input.uv);
		color.w *= alphaMultipliers[2] * lifeLeft;
		color *= colorModifiers[2];
		saturate(color);
	}
	
	return color;
}