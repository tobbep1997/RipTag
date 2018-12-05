cbuffer destructionTimer : register(b0)
{
    float4 TimerAndForwardVector;
    float4x4 lastPos;
    float4x4 worldMatrixInverse;
    float4x4 worldMatrix;

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


//	vec3 n = vec3(0.0, 0.0, 1.0); // the axis to rotate about
//
//// Specify the rotation transformation matrix:
//	mat3 m = mat3(
//		axis.x*axis.x * (1.0f - cos(angle)) + cos(angle),       // column 1 of row 1
//		axis.x*axis.y * (1.0f - cos(angle)) + axis.z * sin(angle), // column 2 of row 1
//		axis.x*axis.z * (1.0f - cos(angle)) - axis.y * sin(angle), // column 3 of row 1
//
//		axis.y*axis.x * (1.0f - cos(angle)) - axis.z * sin(angle), // column 1 of row 2
//		axis.y*axis.y * (1.0f - cos(angle)) + cos(angle),       // ...
//		axis.y*axis.z * (1.0f - cos(angle)) + axis.x * sin(angle), // ...
//
//		axis.z*axis.x * (1.0f - cos(angle)) + axis.y * sin(angle), // column 1 of row 3
//		axis.z*axis.y * (1.0f - cos(angle)) - axis.x * sin(angle), // ...
//		axis.z*axis.z * (1.0f - cos(angle)) + cos(angle)        // ...
//	);

    float4x4 rotationMatrix = 0;
    rotationMatrix._11 = axis.x*axis.x * (1.0f - cos(angle)) + cos(angle);
    rotationMatrix._12 = axis.x*axis.y * (1.0f - cos(angle)) + axis.z * sin(angle);
    rotationMatrix._13 = axis.x*axis.z * (1.0f - cos(angle)) - axis.y * sin(angle);
	rotationMatrix._14 = 0.0f;

    rotationMatrix._21 = axis.y*axis.x * (1.0f - cos(angle)) - axis.z * sin(angle);
    rotationMatrix._22 = axis.y*axis.y * (1.0f - cos(angle)) + cos(angle);
    rotationMatrix._23 = axis.y*axis.z * (1.0f - cos(angle)) + axis.x * sin(angle);
	rotationMatrix._24 = 0.0f;

    rotationMatrix._31 = axis.z*axis.x * (1.0f - cos(angle)) + axis.y * sin(angle);
	rotationMatrix._32 = axis.z*axis.y * (1.0f - cos(angle)) - axis.x * sin(angle);
	rotationMatrix._33 = axis.z*axis.z * (1.0f - cos(angle)) + cos(angle);
	rotationMatrix._34 = 0.0f;

	rotationMatrix._43 = 0.0f;
	rotationMatrix._43 = 0.0f;
	rotationMatrix._43 = 0.0f;
	rotationMatrix._44 = 1.0f;

	return  rotationMatrix;
}

float4x4 createScaleMatrix(float3 scale)
{
	float4x4 scaleMatrix;

	scaleMatrix._11 = scale.x;
	scaleMatrix._12 = 0;
	scaleMatrix._13 = 0;
	scaleMatrix._14 = 0;

	scaleMatrix._21 = 0;
	scaleMatrix._22 = scale.y;
	scaleMatrix._23 = 0;
	scaleMatrix._24 = 0;

	scaleMatrix._31 = 0;
	scaleMatrix._32 = 0;
	scaleMatrix._33 = scale.z;
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
	//float3 forwardVec = TimerAndForwardVector.xyz;
	//float3 right = normalize(cross(float3(0, 1, 0), forwardVec));

	float trianglePosLerpX = 0;
	float trianglePosLerpY = 0.5;
	float scaleLerpX = 1;
	float scaleLerpY = 0.3;
	float rotX = 0;
	float rotY = 3.141592;

	if (lerpValue <= 0.5)
	{
		lerpValue = lerpValue * 2;
	}
	else
	{
		//lerpValue =  1 -(lerpValue - 0.5f) / 1.5f;
		lerpValue = 1 - (lerpValue - 0.5f) * 2;
	}


	float3 offsetNormal = normalize(cross((input[1].worldPos - input[0].worldPos), (input[2].worldPos - input[0].worldPos)));

	float3 centerTriangle = float3((input[0].worldPos.x + input[1].worldPos.x + input[2].worldPos.x) * 0.33,
		(input[0].worldPos.y + input[1].worldPos.y + input[2].worldPos.y) * 0.33,
		(input[0].worldPos.z + input[1].worldPos.z + input[2].worldPos.z) * 0.33);

	//float3 normalizedCenterTriangle = normalize(centerTriangle);


	float moveTriangleLerp = lerp(trianglePosLerpX, trianglePosLerpY, lerpValue);
	
	float scaleLerp = lerp(scaleLerpX, scaleLerpY, lerpValue);
	float4x4 scaleMatrix = createScaleMatrix(float3(scaleLerp, scaleLerp, scaleLerp));

	float3 localVertices[3];
	for (int i = 0; i < 3; i++)
	{
		localVertices[i] = mul(worldMatrixInverse, input[i].worldPos).xyz;
	}

	float3 localOffset = float3((localVertices[0].x + localVertices[1].x + localVertices[2].x) * 0.33,
		(localVertices[0].y + localVertices[1].y + localVertices[2].y) * 0.33,
		(localVertices[0].z + localVertices[1].z + localVertices[2].z) * 0.33); // Get the local offset from the center of the triangle
	float3 localNormal = normalize(cross((localVertices[1] - localVertices[0]), (localVertices[2] - localVertices[0]))); // Get the face normal of the local triangle

	float3 triangleFaceNormal = normalize(mul(worldMatrixInverse, centerTriangle));

	float rotLerp = lerp(rotX, rotY, lerpValue);
	

	for (uint i = 0; i < 3; i ++)
	{
		{
			output.timerValue = TimerAndForwardVector.w;
			output.pos = input[i].pos;
			output.worldPos = input[i].worldPos;
			output.normal = input[i].normal;
			output.TBN = input[i].TBN;
			output.uv = input[i].uv;
			output.color = input[i].color;
			output.info = input[i].info;
		}


		float4 localPos = mul(worldMatrixInverse, output.worldPos); // Get Local vertex

		float3 tempPos = localPos.xyz;

		float3 posAroundOrigin = localPos.xyz - mul(worldMatrixInverse, centerTriangle); // Remove the offset from the vertex so the vertex is around origin
		
		float4 rotatedAroundOrigin = mul(createRotationMatrix(rotLerp, float3(0,1,0)), float4(posAroundOrigin, 1)); // Rotate the local vertex around origin
			   
		
		float3 rotatedLocalPos = rotatedAroundOrigin + mul(worldMatrixInverse, centerTriangle); // Put the vertex back on its local position (tho rotated)

		
		output.worldPos = mul(worldMatrix, float4(rotatedLocalPos, 1));

		

		output.worldPos.w = 1;
		output.pos = mul(output.worldPos, viewProjection);
		outputstream.Append(output);
	}

	outputstream.RestartStrip();

//return outputstream;
}