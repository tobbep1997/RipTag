//SamplerComparisonState sampState : register(s0);
SamplerState sampAniPoint : register(s0);

Texture2DArray txShadowArray : register(t6);

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
	float shadowCoeff = 1;
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

		float epsilon = 0.001;
		
		float width;
		//txShadowArray.GetDimensions(width, width, i);
			
		//float texelSize = 1.0f / width;
		//shadowCoeff = txShadowArray[indexPos];

		float3 indexPos = float3(smTex, i);
		shadowCoeff += (txShadowArray.Sample(sampAniPoint, indexPos).r + 0.001 < depth) ? 0.0f : 1.0f;
		div += 1.0f;
		continue;

		//continue;
		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				//float3 indexPos = float3(smTex + (float2(x, y) * texelSize), i);
				//shadowCoeff += txShadowArray.SampleCmpLevelZero(sampState,indexPos, epsilon).r;
			}
		}
		

		
	}
	
	shadowCoeff /= 1.0f * div;
	
	//shadowCoeff = min(max(shadowCoeff, 0.2), 1.0f);

	return min(dif, float4(1, 1, 1, 1)) * shadowCoeff;

}