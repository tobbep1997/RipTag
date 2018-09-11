
cbuffer LIGHTS : register (b0)
{
	int4	info;
	float4	position[8];
	float4	color[8];
	float	dropOff[8];
}

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

		difMult = max(dot(input.normal, normalize(posToLight.xyz)), 0.0);
		if (difMult > 0)

			dif += attenuation * (saturate(color[i] * input.normal) * difMult);

	}
	return min(dif, float4(1, 1, 1, 1));

}