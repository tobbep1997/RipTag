#pragma once
#include <windows.h>
#include <DirectXMath.h>
#include <fstream>
#define MAX_FILENAME 100

namespace MyLibrary
{
#pragma region OriginalMyLibraryStructs
	struct Vertex
	{
		 float vertex_position[3];
		 float vertex_UVCoord[2];
		 float vertex_normal[3];
		 float vertex_tangent[3];
	};
	struct AnimatedVertex
	{
		float vertex_position[3];
		float vertex_UVCoord[2];
		float vertex_normal[3];
		float vertex_tangent[3];
		unsigned int influencing_joint[4];
		float joint_weights[4];
	};
	struct Transform {
		float transform_position[3];
		float transform_rotation[3];
		float transform_scale[3];

		Transform()
		{
			ZeroMemory(this, sizeof(Transform));
			transform_scale[0] = 1.0;
			transform_scale[1] = 1.0;
			transform_scale[2] = 1.0;
		};
	};
	struct CollisionBox
	{
		float translation[3];
		float scale[3];
		float rotation[3];
	};
	struct CollisionHeader
	{
		int nrOfBoxes;		
	};
	struct CollisionBoxes
	{
		unsigned int nrOfBoxes;
		CollisionBox* boxes;
	};
	struct MeshHeader {

		unsigned int mesh_nrOfVertices;
		char mesh_meshID[MAX_FILENAME];
		//unsigned int mesh_materialID;
	};
	struct MaterialHeader
	{
		unsigned int material_nrOfMaterials;
	};
	struct Material
	{
		char diffuse_pathName[MAX_FILENAME];
		char specular_pathName[MAX_FILENAME];
		char normal_pathName[MAX_FILENAME];
		//char emmissive_pathName[MAX_FILENAME];
	};
	
	struct SkeletonHeader
	{
		unsigned int skeleton_nrOfJoints;
		char skeletonID[MAX_FILENAME];
	};

	struct Joint
	{
		char joint_name[100];
		Transform joint_transform;
		unsigned int parentIndex;
	};

	struct AnimationHeader
	{
		unsigned int anim_nrOfKeys;
	};
	struct AnimKey
	{
		Transform jointTransform;
	};

	//Structs for returning
//--------------------------------------//
	struct VertexFromFile
	{
		float vertex_position[3];
		float vertex_UVCoord[2];
		float vertex_normal[3];
		float vertex_tangent[3];
	};

	struct AnimatedVertexFromFile
	{
		float vertex_position[3];
		float vertex_UVCoord[2];
		float vertex_normal[3];
		float vertex_tangent[3];
		unsigned int influencing_joint[4];
		float joint_weights[4];

		AnimatedVertexFromFile()
		{
			ZeroMemory(this, sizeof(AnimatedVertexFromFile));
		};
	};

	struct AnimatedMeshFromFile
	{
		unsigned int mesh_nrOfVertices;
		char mesh_meshID[MAX_FILENAME];
		//unsigned int mesh_materialID;
		AnimatedVertexFromFile* mesh_vertices;
	};

	struct MeshFromFile
	{
		unsigned int mesh_nrOfVertices;
		char mesh_meshID[MAX_FILENAME];
		//unsigned int mesh_materialID;
		VertexFromFile* mesh_vertices;
	};

	

	struct SkeletonFromFile
	{
		char skeletonID[100];
		unsigned int skeleton_nrOfJoints;
		std::unique_ptr<Joint[]> skeleton_joints;
	};

	struct AnimationFromFile
	{
		unsigned int nr_of_keyframes;
		std::unique_ptr<Transform[]> keyframe_transformations;
	};
#pragma endregion OriginalMyLibraryStructs
#pragma region HelperStructs
	struct Vec4
	{
		float x, y, z, w;

		Vec4(float _x, float _y, float _z, float _w)
			: x(_x), y(_y), z(_z), w(_w)
		{};
		Vec4() {};
	};

	struct DecomposedTransform
	{
		Vec4 translation;
		Vec4 rotation;
		Vec4 scale;
		DecomposedTransform(Vec4 t, Vec4 r = { 0.0, 0.0, 0.0, 0.0 }, Vec4 s = { 1.0, 1.0, 1.0, 1.0 }) : translation(t), rotation(r), scale(s)
		{}
		DecomposedTransform()
		{
			translation = { 0.0, 0.0, 0.0, 1.0 };
			rotation = { 0.0, 0.0, 0.0, 0.0 };
			scale = { 1.0, 1.0, 1.0, 1.0 };
		}
	};

	struct Bone
	{
		DecomposedTransform jointInverseBindPoseTransform;
		DecomposedTransform jointReferenceTransform;

		int32_t parentIndex;

		Bone(DecomposedTransform inverseBindPose, DecomposedTransform referenceTransform, int32_t parent)
			: jointInverseBindPoseTransform(inverseBindPose), jointReferenceTransform(referenceTransform), parentIndex(parent)
		{}
		Bone()
		{}
	};

	struct Skeleton
	{
		std::vector<Bone> joints;
	};

	struct AnimationFromFileStefan
	{
		unsigned int nr_of_keyframes;
		std::unique_ptr<DecomposedTransform[]> keyframe_transformations;
	};
#pragma endregion HelperStructs
}