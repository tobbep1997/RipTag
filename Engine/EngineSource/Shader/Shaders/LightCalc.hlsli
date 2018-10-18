//#include "StaticConstantBuffers.hlsli"
#pragma warning(disable : 3078)
#pragma warning(disable : 3557)
#pragma warning(disable : 3570)
#pragma warning(disable : 3571)
#define PI 3.14159265359

// TODO :: Include this
cbuffer LIGHTS : register(b0)
{
    int4 info; // 16
    float4 lightDropOff[8]; //128
    float4 lightPosition[8]; // 128
    float4 lightColor[8]; //128
}
cbuffer LIGHT_MATRIX : register(b1)
{
    float4x4 lightViewProjection[8][6]; //3072
    int4 numberOfViewProjection[8]; //32
    int4 numberOfLights; //16
};
cbuffer CAMERA_BUFFER : register(b2)
{
    float4 cameraPosition;
    float4x4 viewProjection;
};
cbuffer TEXTURE_BUFFER : register(b7)
{
    int4 usingTexture;
    float4 uvScaling;
    float4 ObjectColor;
};
// end<TODO>


SamplerComparisonState sampAniPoint : register(s0);
SamplerState defaultSampler : register(s1);
SamplerState shadowSampler : register(s2);

Texture2DArray txShadowArray : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);

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
    float k = (r * r) / 8.0f;

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

float4 FresnelReflection(float cosTheta, float4 f0)
{
    return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 512.0f);
}

float4 OptimizedLightCalculation(VS_OUTPUT input, out float4 ambient)
{
    float4 emptyFloat4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 posToLight;
    float distanceToLight;
    float attenuation;
    float4 halfwayVecor;
    float4 radiance;
    float roughnessDistribution;
    float overshadowOcclusion;
    float4 worldToCamera = normalize(float4(cameraPosition.xyz, 1.0f) - float4(input.worldPos.xyz, 1.0f));
    float4 kS, kD;
    float4 numerator;
    float denominator;
    float4 specular;
    float normDotLight;
    float finalShadowCoeff;

    float4 albedo = ObjectColor; //float4(1,1,0,1);
    float3 normal = input.normal.xyz;
    float3 AORoughMet = float3(1, 1, 1); 

    if (usingTexture.x)
    {
        albedo = diffuseTexture.Sample(defaultSampler, input.uv * uvScaling.xy) * ObjectColor;
        normal = normalize(mul((2.0f * normalTexture.Sample(defaultSampler, input.uv * uvScaling.xy).xyz) - 1.0f, input.TBN));
        AORoughMet = MRATexture.Sample(defaultSampler, input.uv * uvScaling.xy).xyz;
    }

    float ao = AORoughMet.x, roughness = AORoughMet.y, metallic = AORoughMet.z;
    float pi = 3.14;


    ambient = float4(0.2f, 0.2f, 0.25f, 1.0f) * albedo * ao;
    //ambient = float4(0.02f, 0.02f, 0.025f, 1.0f) * albedo * ao;
   
    //float4 ambient = float4(0.15f, 0.15f, 0.15f, 1.0f) * albedo;
    float4 finalColor = emptyFloat4;

    float shadowMapWidth;
    int elements, numberOfLevels;
    txShadowArray.GetDimensions(0, shadowMapWidth, shadowMapWidth, elements, numberOfLevels);
    float texelSize = 1.0f / shadowMapWidth;
	
    float4 f0 = float4(0.04f, 0.04f, 0.04f, 1);
    f0 = lerp(f0, albedo, metallic);
    albedo = pow(albedo, float4(2.3f, 2.3f, 2.3f, 2.3f));

    float4 lightCal = float4(0.0f, 0.0f, 0.0f, 1.0f);

    for (int shadowLight = 0; shadowLight < numberOfLights.x; shadowLight++)
    {
        float div = 1.0f;
        float shadowCoeff = 1.0f;
        for (int targetMatrix = 0; targetMatrix < numberOfViewProjection[shadowLight].x; targetMatrix++)
        {
			// Translate the world position into the view space of the shadowLight
            float4 fragmentLightPosition = mul(float4(input.worldPos.xyz, 1), lightViewProjection[shadowLight][targetMatrix]);
			// Get the texture coords of the "object" in the shadow map
            fragmentLightPosition.xyz /= fragmentLightPosition.w;
			// Texcoords are not [-1, 1], change the coords to [0, 1]
            float2 smTex = float2(0.5f * fragmentLightPosition.x + 0.5f, -0.5f * fragmentLightPosition.y + 0.5f);

            float depth = fragmentLightPosition.z;

            if (smTex.x < 0 || smTex.x > 1 || depth < 0.0f || depth > 1.0f)
                continue;

            if (smTex.y < 0 || smTex.y > 1 || depth < 0.0f || depth > 1.0f)
                continue;

            float3 indexPos = float3(smTex, (shadowLight * 6) + targetMatrix);

            float epsilon = 0.001f;

            shadowCoeff += (txShadowArray.Sample(shadowSampler, indexPos).r < depth - epsilon) ? 0.0f : 1.0f;
            div += 1.0f;
            break;
        }

        finalShadowCoeff = pow(shadowCoeff / div, 512);
        posToLight = normalize(lightPosition[shadowLight] - input.worldPos);
        distanceToLight = length(lightPosition[shadowLight] - input.worldPos);
        halfwayVecor = normalize(worldToCamera + posToLight);
        attenuation = (lightDropOff[shadowLight].x / (1.0f + lightDropOff[shadowLight].y * pow(distanceToLight, lightDropOff[shadowLight].z)));
		
        radiance = lightColor[shadowLight] * attenuation;
		 
        roughnessDistribution = RoughnessDistribution(normal, halfwayVecor.xyz, roughness);
        overshadowOcclusion = OvershadowOcclusion(normal, worldToCamera.xyz, posToLight.xyz, roughness);
		
        kS = FresnelReflection(max(dot(halfwayVecor, worldToCamera), 0.0f), f0);
        kD = float4(1.0f, 1.0f, 1.0f, 1.0f) - kS;
        kD *= 1.0f - metallic;
		
		
        numerator = roughnessDistribution * overshadowOcclusion * kS;
        denominator = 4.0f * max(dot(normal, worldToCamera.xyz), 0.0f) * max(dot(normal, posToLight.xyz), 0.0f);
        specular = numerator / max(denominator, 0.001f);
		
        normDotLight = max(dot(normal, posToLight.xyz), 0.0f);
        lightCal += finalShadowCoeff * (kD * albedo / pi + specular) * radiance * normDotLight * ((lightDropOff[shadowLight].x - attenuation + 1.0f));
    }
    
    finalColor = ambient + lightCal;
	
    // why do we need this?
    finalColor = saturate(finalColor);
    //finalColor = finalColor / (finalColor + float4(1.0f, 1.0f, 1.0f, 1.0f));
    //finalColor = pow(abs(finalColor), float4(0.45f, 0.45f, 0.45f, 0.45f));
    finalColor.a = albedo.a;
    return min(finalColor, float4(1, 1, 1, 1));
}