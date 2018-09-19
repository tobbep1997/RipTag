#pragma warning(disable : 3078)
#pragma warning(disable : 3570)
#pragma error(disable : 3662)
SamplerState defaultSampler : register(s1);

Texture2DArray txShadowArray : register(t0);
RWTexture2D<uint> OutputMap : register(u1);
cbuffer LIGHTS : register (b0)
{
	int4	info; // 16
	float4	lightDropOff[8];
	float4	lightPosition[8]; // 128
	float4	lightColor[8]; //128
}

cbuffer LIGHT_MATRIX : register(b1)
{
	float4x4 lightViewProjection[8][6];
	int numberOfLights;
};

struct VS_OUTPUT
{
	
	float4 worldPos : SV_POSITION;

};

void realUAVCalc(VS_OUTPUT input)
{
    float width;
    int dum, dumbdumb;
    txShadowArray.GetDimensions(0, width, width, dum, dumbdumb);
    bool lit = false;
    for (int light = 0; light < numberOfLights && !lit; light++)
    {
        float3 posToLight = lightPosition[light].xyz - input.worldPos.xyz;
        float distanceToLight = length(posToLight);
		
        for (int targetMatrix = 0; targetMatrix < 6 && !lit; targetMatrix++)
        {
			 
            float4 lightView = mul(input.worldPos, lightViewProjection[light][targetMatrix]); // Translate the world position into the view space of the light
            lightView.xyz /= lightView.w; // Get the texture coords of the "object" in the shadow map

            float2 smTex = float2(0.5f * lightView.x + 0.5f, -0.5f * lightView.y + 0.5f); // Texcoords are not [-1, 1], change the coords to [0, 1]

            float depth = lightView.z;

            if (abs(lightView.x) > 1.0f || depth <= 0)
                continue;

            if (abs(lightView.y) > 1.0f || depth <= 0)
                continue;

            float3 indexPos = float3(smTex, (light * 6) + targetMatrix);

            lit = (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? true : false;
            if (lit)
            {
                InterlockedAdd(OutputMap[int2(0, 0)], 1);

            }
			 
				 
        }
		
    }
}
//void lolUAVCalc(VS_OUTPUT input)
//{
//    ////OutputMap[int2(0, 0)];
//    //if (OutputMap[int2(0, 0)] < 5000)
//    //{
//    //    float width;
//    //    int dum, dumbdumb;
//    //    txShadowArray.GetDimensions(0, width, width, dum, dumbdumb);
//    //    bool lit = false;
//    //    for (int light = 0; light < numberOfLights && !lit; light++)
//    //    {
//    //        float3 posToLight = lightPosition[light].xyz - input.worldPos.xyz;
//    //        float distanceToLight = length(posToLight);
		
//    //        for (int targetMatrix = 0; targetMatrix < 6 && !lit; targetMatrix++)
//    //        {
			 
//    //            float4 lightView = mul(input.worldPos, lightViewProjection[light][targetMatrix]); // Translate the world position into the view space of the light
//    //            lightView.xyz /= lightView.w; // Get the texture coords of the "object" in the shadow map

//    //            float2 smTex = float2(0.5f * lightView.x + 0.5f, -0.5f * lightView.y + 0.5f); // Texcoords are not [-1, 1], change the coords to [0, 1]

//    //            float depth = lightView.z;

//    //            if (abs(lightView.x) > 1.0f || depth <= 0)
//    //                continue;

//    //            if (abs(lightView.y) > 1.0f || depth <= 0)
//    //                continue;

//    //            float3 indexPos = float3(smTex, (light * 6) + targetMatrix);

//    //            lit = (txShadowArray.Sample(defaultSampler, indexPos).r < depth - 0.01f) ? true : false;
//    //            if (lit)
//    //            {
//    //                InterlockedAdd(OutputMap[int2(0, 0)], 1);

//    //            }
			 
				 
//    //        }
		
//    //    }
//    //}
//}
void main (VS_OUTPUT input)
{
    //lolUAVCalc(input);
    realUAVCalc(input);

}