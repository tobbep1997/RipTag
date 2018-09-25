cbuffer OBJECT_BUFFER : register(b0)
{
	float4x4 worldMatrix;
};

cbuffer CAMERA_BUFFER : register(b1)
{
	float4 cameraPosition;
	float4x4 viewProjection;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : UV;
};

/*struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;
};*/

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD;
	float4 normal : NORMAL;
	float3x3 TBN : TBN;
	float2 uv : UV;
};


VS_OUTPUT main(VS_INPUT input)
{

	VS_OUTPUT output;



	output.pos = mul(input.pos, mul(worldMatrix, viewProjection));
	output.worldPos = mul(input.pos, worldMatrix);
	output.normal = normalize(mul(input.normal, worldMatrix));
	float3 newTan = normalize(mul(input.tangent, worldMatrix).xyz);
	float3 bitangent = cross(output.normal.xyz, newTan);
	float3x3 TBN = float3x3(newTan, output.normal.xyz, bitangent);
	output.TBN = TBN;
	output.uv = input.uv;
	return output;
}

//cbuffer OBJECT_BUFFER : register(b0)
//{
//	float4x4 worldMatrix;
//	float4x4 skinningMatrices[256];
//};
//
//cbuffer CAMERA_BUFFER : register(b1)
//{
//	float4x4 viewProjection;
//};
//
//struct VS_INPUT
//{
//	float4 pos : POSITION;
//	float4 normal : NORMAL;
//	float4 tangent : TANGENT;
//	float4 weights : WEIGHTS;
//	uint4 boneIndices : BONE;
//	float2 uv : UV;
//	
//};
//
//struct VS_OUTPUT
//{
//	float4 pos : SV_POSITION;
//	float4 worldPos : WORLD;
//	float4 normal : NORMAL;
//	float4 tangent : TANGENT;
//	float2 uv : UV;
//};
//
//VS_OUTPUT main(VS_INPUT input)
//{
//
//	VS_OUTPUT output;
//
//	//init array
//	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	weights[0] = input.weights.x;
//	weights[1] = input.weights.y;
//	weights[2] = input.weights.z;
//	weights[3] = input.weights.w;
//
//
//	//Blend verts
//	float3 position = float3(0.0f, 0.0f, 0.0f);
//	float3 nor = float3(0.0f, 0.0f, 0.0f);
//
//	for (int i = 0; i < 4; i++)
//	{
//		if (input.boneIndices[i] >= 0) //unused bone indices are negative
//		{
//			//TODO: cbuffer gWorld
//			position += weights[i] * mul(float4(input.pos, 1.0f), skinningMatrices[input.boneIndices[i]]).xyz;
//			nor += weights[i] * mul(input.nor,
//				(float3x3)gBoneTransforms[input.boneIndices[i]]);
//		}
//	}
//
//	output.pos = mul(position, mul(worldMatrix, viewProjection));
//	output.worldPos = mul(position, worldMatrix);
//	output.normal = mul(input.normal, worldMatrix);
//	output.tangent = mul(input.tangent, worldMatrix);
//	output.uv = input.uv;
//	return output;
//}