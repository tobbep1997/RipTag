cbuffer destructionTimer : register(b0)
{
    float4x4 lastPos;
    float4x4 worldMatrix;
    float4 TimerAndForwardVector;
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
float4x4 inverse(float4x4 m) {
	float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
	float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
	float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
	float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

	float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
	float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
	float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
	float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

	float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
	float idet = 1.0f / det;

	float4x4 ret;

	ret[0][0] = t11 * idet;
	ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
	ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
	ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

	ret[1][0] = t12 * idet;
	ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
	ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
	ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

	ret[2][0] = t13 * idet;
	ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
	ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
	ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

	ret[3][0] = t14 * idet;
	ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
	ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
	ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

	return ret;
}
[maxvertexcount(3)]
void main(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputstream)
{
	GS_OUTPUT output = (GS_OUTPUT)0;

	float lerpValue = TimerAndForwardVector.w;

	float trianglePosLerpX = 0;
	float trianglePosLerpY = 0.5;
	float scaleLerpX = 1;
	float scaleLerpY = 0.3;
	float rotX = 0;
	float rotY = 3.141592 * 2;

	
	float4x4 from;
	float4x4 to;

	if (lerpValue <= 0.5)
	{
		lerpValue = lerpValue * 2;
		from = lastPos;
		to = lastPos;
	}
	else
	{
		//lerpValue =  1 -(lerpValue - 0.5f) / 1.5f;
		lerpValue = 1 - (lerpValue - 0.5f) * 2;
		 rotY = 0;
		 rotX = 3.141592 * 2;
		 		 
		 to = lastPos;
		 from = worldMatrix;
	}
	float4x4 finalWorldPos;
	float4x4 finalWorldPosInverse;
	finalWorldPos = lerp(from, to, lerpValue);
	finalWorldPosInverse = inverse(finalWorldPos);

	float3 offsetNormal = normalize(cross((input[1].worldPos - input[0].worldPos), (input[2].worldPos - input[0].worldPos)));

	float3 centerTriangle = float3(
		(input[0].worldPos.x + input[1].worldPos.x + input[2].worldPos.x),
		(input[0].worldPos.y + input[1].worldPos.y + input[2].worldPos.y),
		(input[0].worldPos.z + input[1].worldPos.z + input[2].worldPos.z)
		);
	centerTriangle /= 3.0f;

	const float3 localOffset = mul(finalWorldPosInverse, float4(centerTriangle, 1.0f));

	const float3 triangleFaceNormal = normalize(localOffset);

	const float rotLerp = lerp(rotX, rotY, lerpValue);
	
	const float4 localCenter = float4(0.f, 1.f, 0.f, 1.f);

	float4 lerpTriPosTowards = localCenter;

	float4 modelPos = float4(finalWorldPos._41, finalWorldPos._42 + 1.5f, finalWorldPos._43, 1.0f);

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


		float4 localPos = mul(finalWorldPosInverse, output.worldPos); // Get Local vertex
		

		float3 posAroundOrigin = localPos.xyz - localOffset; // Remove the offset from the vertex so the vertex is around origin
		

		float4 rotatedAroundOrigin = mul(float4(posAroundOrigin, 1), createRotationMatrix(rotLerp, triangleFaceNormal)); // Rotate the local vertex around origin
			   
		float3 rotatedLocalPos = rotatedAroundOrigin.xyz + localOffset; // Put the vertex back on its local position (tho rotated)


	
		output.worldPos = mul(finalWorldPos, float4(rotatedLocalPos, 1)); // Go to  worldSpace

		
		output.worldPos.xyz = lerp(output.worldPos.xyz, modelPos.xyz + normalize(output.worldPos.xyz - modelPos.xyz + offsetNormal) * 1.f, lerpValue);


		output.worldPos.w = 1;
		output.pos = mul(output.worldPos, viewProjection);
	
		outputstream.Append(output);
	}

	outputstream.RestartStrip();

//return outputstream;
}