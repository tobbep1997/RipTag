#pragma warning(disable : 3078)

SamplerComparisonState sampAniPoint : register(s0);
SamplerState defaultSampler : register(s1);

Texture2DArray txShadowArray : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);

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
	float3x3 TBN : TBN;
	float2 uv : UV;
};

float RoughnessDistribution(float3 N, float3 H, float roughness)
{
	float r4 = (roughness * roughness) * (roughness * roughness);
	float nDotH = max(dot(N, H), 0.0f);
	float nDotH_2 = nDotH * nDotH;

	float denom = (nDotH_2 * (r4 - 1.0f) + 1.0f);
	denom = 3.14f * denom * denom;

	return r4 / max(denom, 0.001f);
}

float Occlusion(float normDotView, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r*r) / 8.0f;

	float denom = normDotView * (1.0f - k) + k;

	return normDotView / denom;
}

float OvershadowOcclusion(float3 normal, float3 view, float3 light, float roughness)
{
	float normDotView = max(dot(normal, view), 0.0f);
	float normDotLight = max(dot(normal, light), 0.0f);
	float occlusionView = Occlusion(normDotView, roughness);
	float occlusionLight = Occlusion(normDotLight, roughness);

	return occlusionView * occlusionLight;
}

float3 FresnelReflection(float cosTheta, float3 f0)
{
	return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float3 albedo = diffuseTexture.Sample(defaultSampler, input.uv).xyz;
	float3 N = mul(normalTexture.Sample(defaultSampler, input.uv).xyz, input.TBN);
	float3 AORoughMet = MRATexture.Sample(defaultSampler, input.uv).xyz;
	float ao = AORoughMet.x, roughness = AORoughMet.y, metallic = AORoughMet.z;
	float pi = 3.14;
	float3 posToLight;
	float3 H;
	float distanceToLight;
	float attenuation;
	float3 radiance;
	float NDF;
	float G;
	float3 V = normalize(cameraPosition.xyz - input.worldPos.xyz);
	float3 kS, kD;
	float3 numerator;
	float denominator;
	float3 specular;
	float normDotLight;

	float3 f0 = float3(0.04f, 0.04f, 0.04f);
	f0 = lerp(f0, albedo, metallic);
	albedo = pow(albedo, float3(2.3f, 2.3f, 2.3f));

	float3 lightCal = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < info.x; i++)
	{
		posToLight = normalize(lightPosition[i].xyz - input.worldPos.xyz);
		distanceToLight = length(posToLight);
		H = normalize(V + posToLight);
		attenuation = 1.0f / (distanceToLight * distanceToLight);
		radiance = lightColor[i].xyz * attenuation;

		NDF = RoughnessDistribution(N, H, roughness);
		G = OvershadowOcclusion(N, V, posToLight, roughness);

		kS = FresnelReflection(max(dot(H, V), 0.0f), f0);
		kD = float3(1.0f, 1.0f, 1.0f) - kS;
		kD *= 1.0f - metallic;

		numerator = NDF * G * kS;
		denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, posToLight), 0.0f);
		specular = numerator / max(denominator, 0.001f);

		normDotLight = max(dot(N, posToLight), 0.0f);
		lightCal += (kD * albedo / pi + specular) * radiance * normDotLight;
	}

	float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;
	float3 finalColor = ambient + lightCal;

	finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
	finalColor = pow(abs(finalColor), float3(0.45f, 0.45f, 0.45f));


	return float4(finalColor, 1.0f);

}