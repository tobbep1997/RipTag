//#include "StaticConstantBuffers.hlsli"
#pragma warning(disable : 3078)
#pragma warning(disable : 3557)
#pragma warning(disable : 3570)
#pragma warning(disable : 3571)
#define PI  3.14159265359
#define E   2.71828
// TODO :: Include this
cbuffer LIGHTS : register(b0)
{
    int4   info; // 16
    float4 lightDropOff[8]; //128
    float4 lightPosition[8]; // 128
    float4 lightColor[8]; //128 
};
cbuffer LIGHT_MATRIX : register(b1)
{
    float4x4 lightViewProjection[8][6]; //3072
    int4 numberOfViewProjection[8]; //32
    int4 numberOfLights; //16
    uint4 useDir[8][6]; //16
};
cbuffer CAMERA_BUFFER : register(b2)
{
    float4 cameraPosition;
    float4x4 viewProjection;
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

	float4 color : COLOR;
	int4 info : INFO;
};


float RoughnessDistribution(float3 N, float3 H, float roughness)
{
    float r4 = (roughness * roughness) * (roughness * roughness);
    float nDotH = max(dot(N, H), 0.0f);
    float nDotH_2 = nDotH * nDotH;

    float denom = (nDotH_2 * (r4 - 1.0f) + 1.0f);
    denom = 3.141592f * denom * denom;

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
    return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.f);
}

bool calculateShadows(in VS_OUTPUT input, in int shadowLight, in int targetMatrix, inout float shadowCoeff, inout float shadowDivider, inout float4 specular)
{
    if (useDir[shadowLight][targetMatrix].x != 0)
    {
        float4 posToLight = normalize(lightPosition[shadowLight] - input.worldPos);
        
        //----------------------------------------------------------------
		// Translate the world position into the view space of the shadowLight
        float4 fragmentLightPosition = mul(float4(input.worldPos.xyz, 1), lightViewProjection[shadowLight][targetMatrix]);
		// Get the texture coords of the "object" in the shadow map
        fragmentLightPosition.xyz /= fragmentLightPosition.w;
		// Texcoords are not [-1, 1], change the coords to [0, 1]
        float2 smTex = float2(0.5f * fragmentLightPosition.x + 0.5f, -0.5f * fragmentLightPosition.y + 0.5f);
        //The depth of the fragment relativ to the light
        float depth = fragmentLightPosition.z;

        //----------------------------------------------------------------
        //This checks if the fragment is outside the shadowmap
        //If its outside we dont want to sample from that texture
        if (smTex.x < 0 || smTex.x > 1 || depth < 0.0f || depth > 1.0f ||
                    smTex.y < 0 || smTex.y > 1 || depth < 0.0f || depth > 1.0f)            
            return false;

        //----------------------------------------------------------------
        //Here we calculate the epsilon value
        float margin = max(dot(input.normal.xyz, posToLight.xyz), 0.001);
        float epsilon = 0.000125 / margin;
        epsilon = clamp(epsilon, 0, 0.1);
        //epsilon = .0001f;
        //----------------------------------------------------------------
        //Because we are sampling from a Texture2DArray we need a float3 to sample from it
        //float3(u, v, index)
        float3 indexPos = float3(smTex, (shadowLight * 6) + targetMatrix);

        //----------------------------------------------------------------
        //Get the texelSize
        float width, element;
        txShadowArray.GetDimensions(width, width, element);
        float texelSize = 1.0f / width;

        //----------------------------------------------------------------
        //Here we sample from the shadowmap 
        //We sample arount the current texel and avrage out the values
        float currentShadowCoeff;
        float divider = 1.0f;
        int sampleSize = 1;
        for (int x = -sampleSize; x <= sampleSize; ++x)
        {
            for (int y = -sampleSize; y <= sampleSize; ++y)
            {
                currentShadowCoeff += float(txShadowArray.SampleCmpLevelZero(sampAniPoint, float3(smTex + (float2(x, y) * texelSize), (shadowLight * 6) + targetMatrix), depth - epsilon).r);
                divider += 1.0f;
            }
        }

        //----------------------------------------------------------------
        //Here is the final part of the shadowmap 
        //We add and avrage out the values and mult the shadow with specular to help it to not shine through walls 
        currentShadowCoeff /= divider;
        specular *= currentShadowCoeff;
        shadowCoeff += currentShadowCoeff;
        shadowDivider += 1.0f;           
        
        return true;
    }
    
    return false;
}

float4 GuardOptimizedLightCalculation(VS_OUTPUT input, out float4 ambient)
{
    /*
        The only difference between the guard and the regular light calculations is that the guards dont have fog.
        This is because the fog counts as the "player" and the amount of fragments that are drawn are way to high.
    */
    //----------------------------------------------------------------  Vars
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
	
    //----------------------------------------------------------------  
	/*
        This is the default values 
        We need these if there is no textures. (Albedo, Normal, ORM)
    */    
    float4 albedo = input.color;            //Albedo
    float3 normal = input.normal.xyz;       //Normal
    float3 AORoughMet = float3(1, 1, 1);    //ORM

    ambient = float4(.09f, .09f, .09f, 1);  //Ambient default
                                            //We need to set the ambient becouse its and output parameter                                            
    //----------------------------------------------------------------
    //We flip the v becouse export and shit
    input.uv.y = 1 - input.uv.y;

    //----------------------------------------------------------------	
    /*
        input.info.x Says if the object got a texture assigned 
	    and if not we will not sample the textures
    */
    if (input.info.x)
    {
        /*
            We still want the color of the object if it is changed by the user
            The default value on the color is (1,1,1,1)
        */
        albedo = diffuseTexture.Sample(defaultSampler, input.uv) * input.color;
        /*
            Here we sample the normal texture and do stuff
            I honestly got no fucking clue whats happening here
        */
        normal = normalize(input.normal.xyz + mul((2.0f * normalTexture.Sample(defaultSampler, input.uv).xyz - 1.0f), input.TBN));
        /*
            This is the texture that makes stuff shiny
            Very important texture
        */
        AORoughMet = MRATexture.Sample(defaultSampler, input.uv).xyz;
    }

    //----------------------------------------------------------------
    /*
        Don't touch the magic varibles
        Just don't
    */
    float ao = AORoughMet.x,
    roughness = AORoughMet.y,
    metallic = AORoughMet.z;

    ambient = ambient * albedo * ao;
    float4 finalColor = float4(0, 0, 0, 0);
	
    float4 f0 = float4(0.04f, 0.04f, 0.04f, 1);
    f0 = lerp(f0, albedo, metallic);
    albedo = pow(albedo, float4(2.3f, 2.3f, 2.3f, 2.3f));

    //----------------------------------------------------------------
    /*
        Here the fun begins and we start to sample the shadow maps
        The amount of shadow maps is always between [0, 48] in intervalls of 6
        This is because of the pointlights, and they need a camera for every direction possible.
        up, down, left, right, front, back
    */
    float4 lightCal = float4(0.0f, 0.0f, 0.0f, 1.0f); //This is the final color
    for (int shadowLight = 0; shadowLight < numberOfLights.x; shadowLight++)
    {
        //----------------------------------------------------------------
        //  These are for calculating the avrage of all the shadows 
        float div = 1.0f;
        float shadowCoeff = 1.0f;
        
        //----------------------------------------------------------------
        //  Specular stuff
        posToLight = normalize(lightPosition[shadowLight] - input.worldPos);
        distanceToLight = length(lightPosition[shadowLight] - input.worldPos);
        halfwayVecor = normalize(worldToCamera + posToLight);
        
        //----------------------------------------------------------------
        //  The attenuation is the light dropoff for pointlights
        attenuation = ((lightDropOff[shadowLight].x) / (1.0f + lightDropOff[shadowLight].y * pow(distanceToLight, lightDropOff[shadowLight].z)));
        radiance = lightColor[shadowLight] * attenuation;
		 
        //----------------------------------------------------------------
        /*
            Stuff that make stuff shiny and reflectiv
        */
        roughnessDistribution = RoughnessDistribution(normal, halfwayVecor.xyz, roughness);
        overshadowOcclusion = OvershadowOcclusion(normal, worldToCamera.xyz, posToLight.xyz, roughness);

        kS = FresnelReflection(max(dot(halfwayVecor, worldToCamera), 0.0f), f0);
        kD = float4(1.0f, 1.0f, 1.0f, 1.0f) - kS;
        kD *= 1.0f - metallic;

        numerator = roughnessDistribution * overshadowOcclusion * kS;
        denominator = 4.0f * max(dot(normal, worldToCamera.xyz), 0.0f) * max(dot(normal, posToLight.xyz), 0.0f);

        /*
            Hey wait you say, That looks alot like the attenuation calculations.    
            Well observed sir. We can't use the previous one because its not between [0, 1]
        */
        specular = (numerator / max(denominator, 0.001f)) * (1.0f / (1.0f + 0.1f * pow(distanceToLight, 2.0f)));
        //----------------------------------------------------------------

        for (int targetMatrix = 0; targetMatrix < numberOfViewProjection[shadowLight].x; targetMatrix++)
        {
            if (calculateShadows(input, shadowLight, targetMatrix, shadowCoeff, div, specular))
            {
                break;
            }
        }
        //----------------------------------------------------------------
        //Okey i lied 
        //This is the final final part of the shadowmap
        finalShadowCoeff = pow(shadowCoeff / div, 32);
        
        //----------------------------------------------------------------
        //Mult everything together to get the final pixel color
        normDotLight = max(dot(normal, posToLight.xyz), 0.0f);
        lightCal += finalShadowCoeff * (kD * albedo / PI + specular) * radiance * normDotLight * ((lightDropOff[shadowLight].x - attenuation + 1.0f));
                
    }
    //----------------------------------------------------------------
    //Add ambient and reset the Alpha (that we don't support)
    finalColor = ambient + lightCal;
    finalColor.a = albedo.a;

    //----------------------------------------------------------------
    //Return
    return min(finalColor, float4(1, 1, 1, 1));
}

float4 OptimizedLightCalculation(VS_OUTPUT input, out float4 ambient)
{	
    //----------------------------------------------------------------  Vars
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
	
    //----------------------------------------------------------------  
	/*
        This is the default values 
        We need these if there is no textures. (Albedo, Normal, ORM)
    */    
    float4 albedo = input.color;            //Albedo
    float3 normal = input.normal.xyz;       //Normal
    float3 AORoughMet = float3(1, 1, 1);    //ORM

    ambient = float4(.09f, .09f, .09f, 1);  //Ambient default
    ambient = float4(.5, .5, .5, 1); //We need to set the ambient becouse its and output parameter                                            
    //----------------------------------------------------------------
    //We flip the v becouse export and shit
    input.uv.y = 1 - input.uv.y;

    //----------------------------------------------------------------	
    /*
        input.info.x Says if the object got a texture assigned 
	    and if not we will not sample the textures
    */
    if (input.info.x)
    {   
        /*
            We still want the color of the object if it is changed by the user
            The default value on the color is (1,1,1,1)
        */
        albedo = diffuseTexture.Sample(defaultSampler, input.uv) * input.color;
        /*
            Here we sample the normal texture and do stuff
            I honestly got no fucking clue whats happening here
        */
        normal = normalize(input.normal.xyz + mul((2.0f * normalTexture.Sample(defaultSampler, input.uv).xyz - 1.0f), input.TBN));
        /*
            This is the texture that makes stuff shiny
            Very important texture
        */
        AORoughMet = MRATexture.Sample(defaultSampler, input.uv).xyz;
    } 
    //return float4(normal, 1);
    //----------------------------------------------------------------
    /*
        Don't touch the magic varibles
        Just don't
    */
    float ao = AORoughMet.x, 
    roughness = AORoughMet.y, 
    metallic = AORoughMet.z;

    ambient = ambient * albedo * ao;
    float4 finalColor = float4(0,0,0,0);
	
    float4 f0 = float4(0.04f, 0.04f, 0.04f, 1);
    f0 = lerp(f0, albedo, metallic);
    albedo = pow(albedo, float4(2.3f, 2.3f, 2.3f, 2.3f));

    //----------------------------------------------------------------
    /*
        Here the fun begins and we start to sample the shadow maps
        The amount of shadow maps is always between [0, 48] in intervalls of 6
        This is because of the pointlights, and they need a camera for every direction possible.
        up, down, left, right, front, back
    */
    float4 lightCal = float4(0.0f, 0.0f, 0.0f, 1.0f);   //This is the final color
    for (int shadowLight = 0; shadowLight < numberOfLights.x; shadowLight++)
    {
        //----------------------------------------------------------------
        //  These are for calculating the avrage of all the shadows 
        float div = 1.0f;
        float shadowCoeff = 1.0f;
        
        //----------------------------------------------------------------
        //  Specular stuff
        posToLight = normalize(lightPosition[shadowLight] - input.worldPos);
        distanceToLight = length(lightPosition[shadowLight] - input.worldPos);
        halfwayVecor = normalize(worldToCamera + posToLight);
        
        //----------------------------------------------------------------
        //  The attenuation is the light dropoff for pointlights
        attenuation = ((lightDropOff[shadowLight].x) / (1.0f + lightDropOff[shadowLight].y * pow(distanceToLight, lightDropOff[shadowLight].z)));
        radiance = lightColor[shadowLight] * attenuation;
		 
        //----------------------------------------------------------------
        /*
            Stuff that make stuff shiny and reflectiv
        */
        roughnessDistribution = RoughnessDistribution(normal, halfwayVecor.xyz, roughness);
        overshadowOcclusion = OvershadowOcclusion(normal, worldToCamera.xyz, posToLight.xyz, roughness);

        kS = FresnelReflection(max(dot(halfwayVecor, worldToCamera), 0.0f), f0);
        kD = float4(1.0f, 1.0f, 1.0f, 1.0f) - kS;
        kD *= 1.0f - metallic;

        numerator = roughnessDistribution * overshadowOcclusion * kS;
        denominator = 4.0f * max(dot(normal, worldToCamera.xyz), 0.0f) * max(dot(normal, posToLight.xyz), 0.0f);

        /*
            Hey wait you say, That looks alot like the attenuation calculations.    
            Well observed sir. We can't use the previous one because its not between [0, 1]
        */
        specular = (numerator / max(denominator, 0.001f)) * (1.0f / (1.0f + 0.08f * pow(distanceToLight, 2.0f)));
        //----------------------------------------------------------------
        for (int targetMatrix = 0; targetMatrix < numberOfViewProjection[shadowLight].x; targetMatrix++)
        {
            if (calculateShadows(input, shadowLight, targetMatrix, shadowCoeff, div, specular))
            {
                break;
            }            
        }
        //----------------------------------------------------------------
        //Okey i lied 
        //This is the final final part of the shadowmap
        finalShadowCoeff = pow(shadowCoeff / div, 32);
        
        //----------------------------------------------------------------
        //Mult everything together to get the final pixel color
        normDotLight = max(dot(normal, posToLight.xyz), 0.0f);
        lightCal += finalShadowCoeff * (kD * albedo / PI + specular) * radiance * normDotLight * ((lightDropOff[shadowLight].x - attenuation + 1.0f));
                
    }
    //----------------------------------------------------------------
    // The distance divided by the farplane
    float fogMul;
    float fogDepth = length(cameraPosition - input.worldPos) / 50.0f;

    //Fog
    //This is for Linear ---------------------------------------------
    //float fogend    = 0.5;
    //float fogStart  = 0.02;
    //fogMul = saturate((fogend - (fogDepth)) / (fogend - fogStart));
    //This is for exponensial-----------------------------------------
    
    float fogDensity = 2.5f;
    //Two diffrent kinds of equations 
    //See https://docs.microsoft.com/en-us/windows/desktop/direct3d9/images/foggraph.png for the graphs
    //EXP
    //fogMul = 1.0f / pow(E, fogDepth * fogDensity);
    //EXP2
    fogMul = 1.0f / pow(E, pow(fogDepth * fogDensity, 2.0f));
    
    //----------------------------------------------------------------
    //C = f * C + (1 - f) * fC
    finalColor = fogMul * (ambient + lightCal) + ((1.0f - fogMul) * float4(.5, .5, .5, 0.6f));
    finalColor.a = albedo.a;

    //----------------------------------------------------------------
    //Return
    return min(finalColor, float4(1, 1, 1, 1));
}