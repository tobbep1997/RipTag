cbuffer destructionTimer : register(b0)
{
	float4 timer;
}


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

struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;

	float4 color : COLOR;
	int4 info : INFO;
};

float3x3 createRoationMatrix(float angle, float3 axis)
{
	//float3x3 rotationMatrix = {
	//	cos(angle) + (pow(axis.x, 2) * (1 - cos(angle)),    
	//	axis.x * axis.y*(1 - cos(angle)) - axis.z *(sin(angle)),
	//	axis.x * axis.z * (1 - cos(angle)) - axis.x * sin(angle)),
    //
	//	axis.y * axis.x * (1 - cos(angle)) + axis.z * sin(angle),
	//	cos(angle) + pow(axis.y, 2) * (1 - cos(angle)),
	//	axis.y*axis.z * (1 - cos(angle)) - axis.x * sin(angle),
    //
	//	axis.z * axis.x * (1 - cos(angle)) - axis.z * sin(angle),
	//	axis.z * axis.y * (1 - cos(angle)) + axis.x * sin(angle),
	//	cos(angle) + pow(axis.z, 2) * (1 - cos(angle))
	//};
    float3x3 rotationMatrix;

    rotationMatrix._11 = cos(angle) + (pow(axis.x, 2) * (1 - cos(angle)));
    rotationMatrix._12 = axis.x * axis.y * (1 - cos(angle)) - axis.z * (sin(angle));
    rotationMatrix._13 = axis.x * axis.z * (1 - cos(angle)) - axis.x * sin(angle);

    rotationMatrix._21 = axis.y * axis.x * (1 - cos(angle)) + axis.z * sin(angle);
    rotationMatrix._22 = cos(angle) + pow(axis.y, 2) * (1 - cos(angle));
    rotationMatrix._23 = axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle);

    rotationMatrix._31 = axis.z * axis.x * (1 - cos(angle)) - axis.z * sin(angle);
    rotationMatrix._32 = axis.z * axis.y * (1 - cos(angle)) + axis.x * sin(angle);
    rotationMatrix._33 = cos(angle) + pow(axis.z, 2) * (1 - cos(angle));

	return  rotationMatrix;

}
[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputstream)
{
	GS_OUTPUT output = (GS_OUTPUT)0;

	float3 offsetNormal = cross((input[1].pos - input[0].pos), (input[2].pos - input[0].pos));


	for (uint i = 0; i < 3; i ++)
	{

		output.pos = input[i].pos;
		output.worldPos = input[i].worldPos;
		output.normal = input[i].normal;
		output.TBN = input[i].TBN;
		output.uv = input[i].uv;
		output.color = input[i].color;
		output.info = input[i].info;

		float oo = abs(sin(timer.x));
		output.pos.xyz += normalize(offsetNormal) * oo;
        output.pos.xyz = mul(output.pos, createRoationMatrix(oo, offsetNormal));
		output.worldPos.xyz += normalize(offsetNormal) * oo;
	

		outputstream.Append(output);
	}

	outputstream.RestartStrip();

//return outputstream;
}