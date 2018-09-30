#pragma once
#include <windows.h>
#include <DirectXMath.h>
#include <fstream>
#define MAX_FILENAME 100
namespace MyLibrary
{
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
	//Not yet 100% sure how to solve the groups, it will depend a bit on how we solve some other things in the game...
	//May be stored by name


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

	///--------------------------------
	// -------- Helper Structs --------
	///---------------v----------------



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

	/////--------------------------------
	//// ------- Helper Functions -------
	/////---------------v----------------

	//// Reads a Vec4 (four floats in x, y, z, w order)
	//Vec4 loadVec4(std::ifstream& file)
	//{
	//	if (!file.is_open())
	//		return Vec4();

	//	Vec4 vec;
	//	file.read((char*)&vec, sizeof(Vec4));

	//	return vec;
	//}

	//// Reads a 32-bit int
	//int32_t loadInt32(std::ifstream& file)
	//{
	//	if (!file.is_open())
	//		return 0;

	//	int32_t int32;
	//	file.read((char*)&int32, sizeof(int32_t));
	//	return int32;
	//}

	//// Reads a DecomposedTransform
	//// (containing 3xVec4 (T, R, S))
	//DecomposedTransform loadTransform(std::ifstream& file)
	//{
	//	if (!file.is_open())
	//		return DecomposedTransform();

	//	DecomposedTransform transform;

	//	transform.translation = (loadVec4(file));
	//	transform.rotation = (loadVec4(file));
	//	transform.scale = (loadVec4(file));

	//	return transform;
	//}

	//// Reads a Bone 
	//// (containing 2xDecomposedTransform and one 32-bit wide int)
	//Bone loadBone(std::ifstream& file)
	//{
	//	if (!file.is_open())
	//		return Bone();

	//	Bone bone;
	//	bone.jointInverseBindPoseTransform = loadTransform(file);
	//	bone.jointReferenceTransform = loadTransform(file);
	//	bone.parentIndex = loadInt32(file);

	//	return bone;
	//}

	//// Reads a Skeleton
	//// (Containing boneCount x Bone)
	//Skeleton loadSkeleton(std::ifstream& file, int32_t boneCount)
	//{
	//	if (!file.is_open())
	//		return Skeleton();

	//	/// Read and add each bone to vector
	//	Skeleton skeleton;
	//	for (int i = 0; i < boneCount; i++)
	//		skeleton.joints.push_back(loadBone(file));

	//	return skeleton;
	//}

	//// Reads a Skeleton
	//// (Containing boneCount x Bone)
	//// No bonecount is supplied; we assume we read that data first (one 32-bit wide int)
	//Skeleton loadSkeleton(std::ifstream& file)
	//{
	//	if (!file.is_open())
	//		return Skeleton();

	//	/// Read bone count
	//	int32_t boneCount = loadInt32(file);

	//	/// Read and add each bone to vector
	//	Skeleton skeleton;
	//	for (int i = 0; i < boneCount; i++)
	//		skeleton.joints.push_back(loadBone(file));

	//	return skeleton;
	//}
}