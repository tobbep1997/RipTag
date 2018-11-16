#pragma once
#include <windows.h>
#include <DirectXMath.h>
#include <fstream>
#define MAX_FILENAME 100

namespace ImporterLibrary
{
#pragma region OriginalImporterLibraryStructs

	
	struct StartingPos
	{
		float startingPos[3];
	};
	struct GridHeader
	{
		int nrOf;
	};
	struct GridPointStruct
	{
		bool pathable;
		float translation[3];
	};
	struct GuardStartingPositions {
		int nrOf;
		StartingPos *startingPositions;
	};
	struct pointLight
	{
		float translate[3];
		float color[3];
		float intensity;
	};
	struct PointLights
	{
		int nrOf;
		pointLight *lights;
	};

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
			std::fill(transform_position, transform_position + 3, 0.0f);
			std::fill(transform_rotation, transform_rotation + 3, 0.0f);
			std::fill(transform_scale, transform_scale + 3, 1.0f);
		};
	};
	struct PropItem
	{
		int typeOfProp;
		int linkedItem;
		bool isTrigger;
		float transform_position[3];
		float transform_rotation[3];
		float transform_scale[3];
		float BBOX_INFO[3];
	};
	struct propsHeader
	{
		int nrOfItems;
	};
	struct PropItemToEngine
	{
		int nrOfItems;
		PropItem * props;
		
	};
	struct GridStruct
	{
		GridPointStruct * gridPoints;
		int nrOf;
		int maxX;
		int maxY;

		GridStruct() { gridPoints = nullptr; }
	};
	struct CollisionBox
	{
		float translation[3];
		float scale[3];
		float rotation[4];
		int typeOfBox;
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
#pragma endregion OriginalImporterLibraryStructs
#pragma region HelperStructs
	struct Vec4
	{
		float x, y, z, w;

		Vec4(float _x, float _y, float _z, float _w)
			: x(_x), y(_y), z(_z), w(_w)
		{};
		Vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {};
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
		Bone() : parentIndex(-1)
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