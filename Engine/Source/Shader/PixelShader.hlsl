#pragma warning(disable : 3078)

SamplerComparisonState sampAniPoint : register(s0);
SamplerState defaultSampler : register(s1);

Texture2DArray txShadowArray : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);

RWTexture2D<uint> OutputMap : register(u1);

cbuffer LIGHTS : register (b0)
{
	int4	info; // 16
	float4	lightDropOff[8];    //128
    float4	lightPosition[8]; // 128
    float4	lightColor[8]; //128
}

cbuffer LIGHT_MATRIX : register(b1)
{	
    float4x4 lightViewProjection[8][6];
    int numberOfLights;
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


float4 OptimizedLightCalculation(VS_OUTPUT input)
{
    float4 emptyFloat4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 textureColor = emptyFloat4;
    textureColor = diffuseTexture.Sample(defaultSampler, float2(input.uv.x, 1.0 - input.uv.y));

    // TEMP VAR
    textureColor = float4(1.0f, 1.0, 1.0f, 1.0f);
    // REMOVE ME WHEN WE HAVE TEXTURES

    float4 ambient = float4(0.05f, 0.05f, 0.05f, 0.0f) * textureColor;
    float3 fragmentPositionToCamera = cameraPosition.xyz - input.worldPos.xyz;
    float4 finalColor = emptyFloat4;

    float shadowMapWidth;
    int elements, numberOfLevels;
    txShadowArray.GetDimensions(0, shadowMapWidth, shadowMapWidth, elements, numberOfLevels);
    float texelSize = 1.0f / shadowMapWidth;
    //if (input.uv.x > 0.1f)
    //{
    //    InterlockedAdd(OutputMap[int2(0, 0)], 1);
    //}
	
    //InterlockedAdd(OutputMap[int2(1, 0)], 1);
	
    for (int light = 0; light < numberOfLights; light++)
    {
        float shadowCoeff = 1.0f;
        float div = 1.0f;
        float3 fragmentPositionToLight = lightPosition[light].xyz - input.worldPos.xyz;
        float fragmentDistanceToLight = length(fragmentPositionToLight.xyz);

        float diffuseDot = max(dot(input.normal.xyz, normalize(fragmentPositionToLight).xyz), 0.0f);
        float specularDot = max(dot(input.normal.xyz, normalize(fragmentPositionToCamera + fragmentPositionToLight).xyz), 0.0f);

        // attenuation = Intensity / (1.0 + lightRange * fragmentDistanceToLight * fragmentDistanceToLight);
        float attenuation = lightDropOff[light].x / (1.0f + lightDropOff[light].y * pow(fragmentDistanceToLight, lightDropOff[light].z));
       // float attenuation = clamp(3.0f / fragmentDistanceToLight, 0.0, 1.0f);
        float3 specular = attenuation * (lightColor[light].rgb * pow(specularDot, 64.0f));
        float4 diffuse = attenuation * (saturate(lightColor[light] * textureColor * diffuseDot));
        for (int targetMatrix = 0; targetMatrix < 6; targetMatrix++)
        {
             // Translate the world position into the view space of the light
            float4 fragmentLightPosition = mul(input.worldPos, lightViewProjection[light][targetMatrix]);
             // Get the texture coords of the "object" in the shadow map
            fragmentLightPosition.xyz /= fragmentLightPosition.w;
             // Texcoords are not [-1, 1], change the coords to [0, 1]
            float2 smTex = float2(0.5f * fragmentLightPosition.x + 0.5f, -0.5f * fragmentLightPosition.y + 0.5f);

            float depth = fragmentLightPosition.z;

            if (smTex.x < 0 || smTex.x > 1 || depth <= 0.0f || depth > 1.0f)
                continue;

            if (smTex.y < 0 || smTex.y > 1|| depth <= 0.0f || depth > 1.0f)
                continue;

            float3 indexPos = float3(smTex, (light * 6) + targetMatrix);
            float tShadow = txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos, depth - 0.01f).r;

            // REMOVE THIS
            //float tShadow2 = (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? 0.0f : 1.0f;

            //float tShadow = (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? 0.0f : 1.0f;
            
            finalColor.rgb += (specular + diffuse.rgb).rgb * tShadow;
            
            //shadowCoeff += tShadow;
            //div += 1.0f;
        }
        //finalColor.rgb += (((specular * pow(shadowCoeff, 2)) + diffuse.rgb) * shadowCoeff);
        //finalColor.rgb += (specular + diffuse.rgb).rgb;
    }
    finalColor.a = textureColor.a;
    return min(ambient + finalColor, float4(1, 1, 1, 1));
}

float4 OldLightCalculation(VS_OUTPUT input)
{
    //VERY TEMP
    float4 color = float4(1, 1, 1, 1);
	//PLEASE REMOBVE
    float4 emptyFloat4 = float4(0, 0, 0, 1);

    float4 txt = float4(0, 0, 0, 1);
    txt.rgb = diffuseTexture.Sample(defaultSampler, float2(input.uv.x, 1.0 - input.uv.y)).rgb;

    //TEMP
    txt = color;

    float4 ambient = float4(0.0, 0.0, 0.0, 1) * txt;
    float4 posToCam = cameraPosition - input.worldPos;
    float4 posToLight = float4(0, 0, 0, 0);

    float specmult = 0;
    float distanceToLight = 0;
    float attenuation = 0;
    float difMult = 0;
    float4 finalColor = float4(0, 0, 0, 1);

    //if (input.uv.x > 0.1f)
    //{
    //    InterlockedAdd(OutputMap[int2(0, 0)], 1);
    //}
	
    //InterlockedAdd(OutputMap[int2(1, 0)], 1);
	
    for (int light = 0; light < numberOfLights; light++)
    {
        float shadowCoeff = 1;
        float div = 1;
        float4 dif = emptyFloat4;
        float4 spec = emptyFloat4;
        posToLight = lightPosition[light] - input.worldPos;
        distanceToLight = length(posToLight);

        attenuation = 1.0 / (1.0 + lightDropOff[light].x * pow(distanceToLight, 2));

        difMult = max(dot((input.normal).xyz, normalize(posToLight).xyz), 0.0f);
        float lolTemp = dot((input.normal).xyz, normalize(posToLight).xyz);
        specmult = dot(input.normal, normalize(posToCam + posToLight)) * (1.0f - lightDropOff[light].x);
        if (specmult > 0)
            spec = lightColor[light] * max(pow(abs(specmult), 32), 0.0f);
        if (difMult > 0)
            dif = attenuation * (saturate(lightColor[light] * txt) * difMult);

        for (int targetMatrix = 0; targetMatrix < 6; targetMatrix++)
        {
            float4 lightView = mul(input.worldPos, lightViewProjection[light][targetMatrix]); // Translate the world position into the view space of the light
            lightView.xy /= lightView.w; // Get the texture coords of the "object" in the shadow map

            float2 smTex = float2(0.5f * lightView.x + 0.5f, -0.5f * lightView.y + 0.5f); // Texcoords are not [-1, 1], change the coords to [0, 1]

            float depth = lightView.z / lightView.w;


            if (abs(lightView.x) > 1.0f || depth <= 0)
                continue;

            if (abs(lightView.y) > 1.0f || depth <= 0)
                continue;

            float3 indexPos = float3(smTex, (light * 6) + targetMatrix);


            float width;
            int dum, dumbdumb;
            txShadowArray.GetDimensions(0, width, width, dum, dumbdumb);

            float texelSize = 1.0f / width;

            const float aa = 0.0f; // FUCK THIS LOL

            for (float x = -aa; x <= aa; x += 1.0f)
            {
                for (float y = -aa; y <= aa; y += 1.0f)
                {
					//shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - 0.01).r;
					//shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - max(0.001, difMult) / distanceToLight).r;
                    shadowCoeff += txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos + (float3(x, y, 0) * texelSize), depth - 0.01f).r;
                    div += 1.0f;

                }

            }

        }
        shadowCoeff /= div;
        finalColor += ((spec + dif) * shadowCoeff);
        
        finalColor.a = 1.0f;
    }
    return min(finalColor, float4(1, 1, 1, 1));
}

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return OptimizedLightCalculation(input);
    //return OldLightCalculation(input);
}