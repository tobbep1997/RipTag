#pragma warning(disable : 3078)

SamplerComparisonState sampAniPoint : register(s0);
//SamplerState sampAniPoint : register(s0);

Texture2DArray txShadowArray : register(t0);

cbuffer LIGHTS : register (b0)
{
	int4	info; // 16
	float4	lightDropOff[8];
    float4	lightPosition[8]; // 128
    float4	lightColor[8]; //128
}

cbuffer LIGHT_MATRIX : register(b1)
{	
	float4x4 lightViewProjection[6];
};
cbuffer CAMERA_BUFFER : register(b2)
{
    float4 cameraPosition;
    float4x4 viewProjection;
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
	//VERY TEMP
    float4 color = float4(1, 1, 1, 1);
	//PLEASE REMOBVE

    float4 dif = float4(0, 0, 0, 1);
    float4 ambient = float4(0.25, 0.25, 0.25, 1) * color;
    float4 posToCam = cameraPosition - input.worldPos;
    float4 posToLight = float4(0, 0, 0, 0);  
    float4 spec = float4(0, 0, 0, 1);
    float specmult = 0;
    float distanceToLight = 0;
    float attenuation = 0;
    float difMult = 0;
	float shadowCoeff = 1;
	float div = 1;

	for (int i = 0; i < info.x; i++)
	{
		posToLight = lightPosition[i] - input.worldPos;
		distanceToLight = length(posToLight);

		attenuation = 1.0 / (1.0 + lightDropOff[i].x * pow(distanceToLight, 2));

		difMult = max(dot((input.normal).xyz, normalize(posToLight).xyz), 0.0f);
		float lolTemp = dot((input.normal).xyz, normalize(posToLight).xyz);
		specmult = dot(input.normal, normalize(posToCam + posToLight)) * (1.0f - lightDropOff[i].x);
		if (difMult > 0)
			dif += attenuation * (saturate(lightColor[i] * color) * difMult);
		if (specmult > 0)
			spec += lightColor[i] * max(pow(abs(specmult), 32), 0.0f);

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

			float3 indexPos = float3(smTex, i);


			float width;
			int dum, dumbdumb;
			txShadowArray.GetDimensions(0, width, width, dum, dumbdumb);

			float texelSize = 1.0f / width;

			const float aa = 1.0f;

			for (float x = -aa; x <= aa; x += 1.0f)
			{
				for (float y = -aa; y <= aa; y += 1.0f)
				{
					//shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - 0.01).r;
					//shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - max(0.001, difMult) / distanceToLight).r;
					shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - (difMult*0.068)).r;
					div += 1.0f;

				}

			}

		}

	}
	shadowCoeff /= div;
	//return float4(shadowCoeff, 0, 0, 1);
	
    return min(ambient + ((spec * shadowCoeff) + dif) * shadowCoeff, float4(1, 1, 1, 1));

}