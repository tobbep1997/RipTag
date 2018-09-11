SamplerState sampState : register(s0);
//SamplerComparisonState sampAniPoint : register(s1);

Texture2D txShadow0 : register(t0);
Texture2D txShadow1 : register(t1);
Texture2D txShadow2 : register(t2);
Texture2D txShadow3 : register(t3);
Texture2D txShadow4 : register(t4);
Texture2D txShadow5 : register(t5);

cbuffer LIGHTS : register (b0)
{
	int4	info;
	float4	position[8];
	float4	color[8];
	float	dropOff[8];
}

cbuffer LIGHT_MATRIX : register(b1)
{
	float4x4 lightViewProjection[6];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 posToLight;
	float  distanceToLight;
	float attenuation;
	float difMult;
	float4 dif = float4(0,0,0,0);

	for (int i = 0; i < info.x; i++)
	{
		posToLight = position[i] - input.worldPos;
		distanceToLight = length(posToLight);


		attenuation = 1.0 / (1.0 + 0.01 * pow(distanceToLight, 2));

		difMult = max(dot(input.normal, normalize(posToLight)), 0.0);
		if (difMult > 0)

			dif += attenuation * (saturate(color[i] * input.normal) * difMult);

	}
	float shadowCoeff = 0;
	float div = 0;
	for (int i = 0; i < 6; i++)
	{

		float4 lightView = mul(input.worldPos, lightViewProjection[i]); // Translate the world position into the view space of the light
		lightView.xy /= lightView.w; // Get the texture coords of the "object" in the shadow map


		float2 smTex = float2(0.5f * lightView.x + 0.5f, -0.5f * lightView.y + 0.5f); // Texcoords are not [-1, 1], change the coords to [0, 1]

		float depth = lightView.z / lightView.w;

		if (abs(lightView.x) > 1.0f || depth <= 0)
			continue;

		if (abs(lightView.y) > 1.0f || depth <= 0)
			continue;

		if (i == 0)
			shadowCoeff += (txShadow0.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		else if (i == 1)
			shadowCoeff += (txShadow1.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		else if (i == 2)
			shadowCoeff += (txShadow2.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		else if (i == 3)
			shadowCoeff += (txShadow3.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		else if (i == 4)
			shadowCoeff += (txShadow4.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		else if (i == 5)
			shadowCoeff += (txShadow5.Sample(sampState, smTex).r + 0.001 < depth) ? 0.2f : 1.0f;
		div += 1.0f;
	}
	
	shadowCoeff /= div;
	
	//shadowCoeff = min(max(shadowCoeff, 0.2), 1.0f);

	return min(dif, float4(1, 1, 1, 1)) * shadowCoeff;

}