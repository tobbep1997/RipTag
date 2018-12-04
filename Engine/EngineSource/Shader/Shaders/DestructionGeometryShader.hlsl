cbuffer destructionTimer : register(b0)
{
    float4 TimerAndForwardVector;
    float4 lastPos;
}

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
	float timerValue : TIMER;
};

float4x4 createRotationMatrix(float angle, float3 axis)
{
    float4x4 rotationMatrix = 0;
    rotationMatrix._11 = cos(angle) + (pow(axis.x, 2) * (1 - cos(angle)));
    rotationMatrix._12 = axis.x * axis.y * (1 - cos(angle)) - axis.z * (sin(angle));
    rotationMatrix._13 = axis.x * axis.z * (1 - cos(angle)) - axis.x * sin(angle);
	rotationMatrix._14 = 0.0f;

    rotationMatrix._21 = axis.y * axis.x * (1 - cos(angle)) + axis.z * sin(angle);
    rotationMatrix._22 = cos(angle) + pow(axis.y, 2) * (1 - cos(angle));
    rotationMatrix._23 = axis.y * axis.z * (1 - cos(angle)) - axis.x * sin(angle);
	rotationMatrix._24 = 0.0f;

    rotationMatrix._31 = axis.z * axis.x * (1 - cos(angle)) - axis.z * sin(angle);
    rotationMatrix._32 = axis.z * axis.y * (1 - cos(angle)) + axis.x * sin(angle);
    rotationMatrix._33 = cos(angle) + pow(axis.z, 2) * (1 - cos(angle));
	rotationMatrix._34 = 0.0f;

	rotationMatrix._43 = 0.0f;
	rotationMatrix._43 = 0.0f;
	rotationMatrix._43 = 0.0f;
	rotationMatrix._44 = 1.0f;

	return  rotationMatrix;
}

float4x4 createScaleMatrix(float scale)
{
	float4x4 scaleMatrix;

	scaleMatrix._11 = 1;
	scaleMatrix._12 = 0;
	scaleMatrix._13 = 0;
	scaleMatrix._14 = 0;

	scaleMatrix._21 = 0;
	scaleMatrix._22 = scale;
	scaleMatrix._23 = 0;
	scaleMatrix._24 = 0;

	scaleMatrix._31 = 0;
	scaleMatrix._32 = 0;
	scaleMatrix._33 = 1;
	scaleMatrix._34 = 0;

	scaleMatrix._41 = 0;
	scaleMatrix._42 = 0;
	scaleMatrix._43 = 0;
	scaleMatrix._44 = 1;


	return  scaleMatrix;
}
float4x4 createTranslationMatrix(float4 posVec)
{
	float4x4 translationMatrix;

	translationMatrix._11 = 1;
	translationMatrix._12 = 0;
	translationMatrix._13 = 0;
	translationMatrix._14 = posVec.x;

	translationMatrix._21 = 0;
	translationMatrix._22 = 1;
	translationMatrix._23 = 0;
	translationMatrix._24 = posVec.y;
	
	translationMatrix._31 = 0;
	translationMatrix._32 = 0;
	translationMatrix._33 = 1;
	translationMatrix._34 = posVec.z;

	translationMatrix._41 = 0;
	translationMatrix._42 = 0;
	translationMatrix._43 = 0;
	translationMatrix._44 = 1;


	return  translationMatrix;
}

[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputstream)
{
	GS_OUTPUT output = (GS_OUTPUT)0;

	float4 startPos = float4(float4(TimerAndForwardVector.x, TimerAndForwardVector.y, TimerAndForwardVector.z, 1));

	float posLerpValue = 0;
	float lerpValue = TimerAndForwardVector.w;
	float trianglePosLerpX = 0;
	float trianglePosLerpY = 1;
	float scaleLerpX = 1;
	float scaleLerpY = 0.3;
	float rotX = 0;
	float rotY = 190;

	if (lerpValue > 1)
	{
		lerpValue = lerpValue - 1;

		trianglePosLerpX = 1;
		trianglePosLerpY = 0;
		posLerpValue = lerpValue;
		scaleLerpY = 1;
		scaleLerpX = 0.3;
		rotY = 0;
		rotX = 190;
	}


	float3 offsetNormal = cross((input[1].pos - input[0].pos), (input[2].pos - input[0].pos));
	float moveTriangleLerp = lerp(trianglePosLerpX, trianglePosLerpY, lerpValue);
	
	float scaleLerp = lerp(scaleLerpX, scaleLerpY, lerpValue);
	float4x4 scaleMatrix = createScaleMatrix(scaleLerp);

	float rotLerp = lerp(rotX, rotY, lerpValue);
	for (uint i = 0; i < 3; i ++)
	{
		output.timerValue = TimerAndForwardVector.w;
		output.pos = input[i].pos;
		output.worldPos = input[i].worldPos;
		output.normal = input[i].normal;
		output.TBN = input[i].TBN;
		output.uv = input[i].uv;
		output.color = input[i].color;
		output.info = input[i].info;

		
		float4 realPos = output.worldPos;
		//////float4 worldPosLerp = lerp(startPos, realPos, posLerpValue);
		output.worldPos = output.worldPos - realPos;
	

		output.worldPos = mul(createRotationMatrix(rotLerp, float3(TimerAndForwardVector.x, TimerAndForwardVector.y, TimerAndForwardVector.z)), output.worldPos);
		
		output.worldPos.xyz += realPos.xyz + (normalize(offsetNormal) * moveTriangleLerp);
		output.worldPos = mul(scaleMatrix, output.worldPos);

		output.worldPos.w = 1;
		//////
		output.pos = mul(output.worldPos, viewProjection);
       // output.pos.y += larpPos;
		outputstream.Append(output);
	}

	outputstream.RestartStrip();

//return outputstream;
}