#pragma warning (disable : 3081)
SamplerState defaultSampler : register(s4);
Texture2D diffuseTexture : register(t1);

cbuffer HUD_TYPE : register (b0)
{
	float4 center;
	uint4 type;
	
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 UV : UV;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	if (type.x == 0)
	{
        if (input.UV.x <= center.x && input.UV.y >= (1.0f - center.y))
		    return diffuseTexture.Sample(defaultSampler, input.UV);
        else
			return (0, 0, 0, 0);
    }
	else if (type.x == 1)
	{
        if (length(float2(0.5f, 0.5f) - input.UV) <= center.z && length(float2(0.5f, 0.5f) - input.UV) >= center.w)
			return diffuseTexture.Sample(defaultSampler, input.UV);
		else
			return (0, 0, 0, 0);


		//Circle
	}
	else
		return float4(1,0,0,1);
}