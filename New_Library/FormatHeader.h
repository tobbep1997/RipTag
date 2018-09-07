#pragma once
#include <windows.h>
#include <DirectXMath.h>

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
		unsigned int influencing_joint[4];
		float joint_weights[4];
	};
	struct Transform {
		float transform_position[3];
		float transform_scale[3];
		float transform_rotation[3];
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
	struct CameraHeader
	{
		unsigned int camera_nrOfCameras;
	};
	struct Camera
	{
		Transform cam_transform;
		float cam_FOV;
	};
	struct LightHeader
	{
		unsigned int light_nrofLights;
	};
	struct Light
	{
		Transform light_transform;
		float light_color[4]; //4th value is intensity
	};
	struct MorphAnimationHeader
	{
		unsigned int morph_nrOfTargets;
		unsigned int morph_meshID[20];
	};
	struct Target
	{
		unsigned int morphTarget_nrOfvertices;
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
	struct GroupHeader
	{
		unsigned int group_itemID;
	};
	struct Group
	{
		char groupItemName[100];
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
		float vertex_biTangent[3];
		unsigned int influencing_joint[4];
		float joint_weights[4];
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

	struct CameraFromFile
	{
		unsigned int cam_nrOfCameras;
		Camera* cam_cameras;
	};

	struct MaterialFromFile
	{
		unsigned int mat_nrOfMaterials;
		Material* mat_materials;
	};

	struct LightFromFile
	{
		unsigned int light_nrofLights;
		Light* light_lights;
	};

	struct MorphAnimationFromFile
	{

	};

	struct SkeletonFromFile
	{
		char skeletonID[100];
		unsigned int skeleton_nrOfJoints;
		Joint* skeleton_joints;
	};

	struct AnimationFromFile
	{
		unsigned int nr_of_keyframes;
		Transform* keyframe_transformations;
	};
}