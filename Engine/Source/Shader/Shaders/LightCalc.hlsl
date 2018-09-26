SamplerComparisonState sampAniPoint : register(s0);
SamplerState defaultSampler : register(s1);

Texture2DArray txShadowArray : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);



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

    //float4 ambient = float4(0.15f, 0.15f, 0.15f, 1.0f) * textureColor;
    float4 ambient = float4(0.0, 0.0, 0.0, 1.0f) * textureColor;
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
	
    for (int light = 0; light < numberOfLights.x; light++)
    {
        float div = 1.0f;
        float shadowCoeff = 1.0f;
		
        float3 fragmentPositionToLight = lightPosition[light].xyz - input.worldPos.xyz;
        float fragmentDistanceToLight = length(fragmentPositionToLight.xyz);
		
        for (int targetMatrix = 0; targetMatrix < numberOfViewProjection[light].x; targetMatrix++)
        {
             // Translate the world position into the view space of the light
            float4 fragmentLightPosition = mul(input.worldPos, lightViewProjection[light][targetMatrix]);
             // Get the texture coords of the "object" in the shadow map
            fragmentLightPosition.xyz /= fragmentLightPosition.w;
             // Texcoords are not [-1, 1], change the coords to [0, 1]
            float2 smTex = float2(0.5f * fragmentLightPosition.x + 0.5f, -0.5f * fragmentLightPosition.y + 0.5f);

            float depth = fragmentLightPosition.z;

            if (smTex.x <= 0 || smTex.x >= 1 || depth <= 0.0f || depth > 1.0f)
                continue;

            if (smTex.y <= 0 || smTex.y >= 1 || depth <= 0.0f || depth > 1.0f)
                continue;

            float3 indexPos = float3(smTex, (light * 6) + targetMatrix);
            //float tShadow = txShadowArray.SampleCmpLevelZero(sampAniPoint, indexPos, depth - 0.01f).r;

            // REMOVE THIS
            //float tShadow2 = (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? 0.0f : 1.0f;

            shadowCoeff += (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? 0.0f : 1.0f;
            div += 1.0f;
            
            
        }

        float diffuseDot = max(dot(input.normal.xyz, normalize(fragmentPositionToLight).xyz), 0.0f);
        float specularDot = max(dot(input.normal.xyz, normalize(fragmentPositionToCamera + fragmentPositionToLight).xyz), 0.0f);
              
        float attenuation = lightDropOff[light].x / (1.0f + lightDropOff[light].y * pow(fragmentDistanceToLight, lightDropOff[light].z));
        attenuation *= (shadowCoeff / div);

        float3 specular = float3(0, 0, 0);
        if (dot(input.normal.xyz, normalize(fragmentPositionToLight)) >= 0)        
            specular = attenuation * (lightColor[light].rgb * pow(specularDot, 32.0f));
        
        float4 diffuse = attenuation * (saturate(lightColor[light] * textureColor * diffuseDot)) * (shadowCoeff / div);

        finalColor.rgb += (specular + diffuse.rgb).rgb * (shadowCoeff / div);
		
    }
    finalColor.a = textureColor.a;
    return min(ambient + finalColor, float4(1, 1, 1, 1));
}
