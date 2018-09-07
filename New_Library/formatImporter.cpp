#include "formatImporter.h"
namespace MyLibrary
{
	Loadera::Loadera()
	{

	}

	Loadera::~Loadera()
	{
	}

	MeshFromFile Loadera::readMeshFile(std::string fileName)
	{
		//reads the custom mesh file and stores all the data

		MeshFromFile meshToReturn;

		bool fileIsOpen = false;

		std::ifstream customMeshFile(fileName, std::ifstream::binary);
		//customMeshFile.open(fileName, std::ifstream::binary);

		if (customMeshFile.is_open())
		{
			fileIsOpen = true;

			MeshHeader meshname;

			//customMeshFile.read((char*)&meshname.mesh_nrOfVertices, sizeof(meshname.mesh_nrOfVertices));
			//customMeshFile.read((char*)&meshname.mesh_meshID, sizeof(meshname.mesh_meshID));
			customMeshFile.read((char*)&meshname, sizeof(MeshHeader));

			meshToReturn.mesh_nrOfVertices = meshname.mesh_nrOfVertices;

			Vertex* vertices = new Vertex[meshname.mesh_nrOfVertices];
			customMeshFile.read((char*)vertices, meshname.mesh_nrOfVertices * sizeof(Vertex));

			meshToReturn.mesh_vertices = new VertexFromFile[meshname.mesh_nrOfVertices];

			for (int i = 0; i < meshname.mesh_nrOfVertices; i++)
			{
				//std::cout << vertices[i].vertex_position[0] << ", " << vertices[i].vertex_position[1] << ", " << vertices[i].vertex_position[2] << std::endl;
				meshToReturn.mesh_vertices[i].vertex_position[0] = vertices[i].vertex_position[0];
				meshToReturn.mesh_vertices[i].vertex_position[1] = vertices[i].vertex_position[1];
				meshToReturn.mesh_vertices[i].vertex_position[2] = vertices[i].vertex_position[2];

				meshToReturn.mesh_vertices[i].vertex_UVCoord[0] = vertices[i].vertex_UVCoord[0];
				meshToReturn.mesh_vertices[i].vertex_UVCoord[1] = vertices[i].vertex_UVCoord[1];

				meshToReturn.mesh_vertices[i].vertex_normal[0] = vertices[i].vertex_normal[0];
				meshToReturn.mesh_vertices[i].vertex_normal[1] = vertices[i].vertex_normal[1];
				meshToReturn.mesh_vertices[i].vertex_normal[2] = vertices[i].vertex_normal[2];

				meshToReturn.mesh_vertices[i].vertex_tangent[0] = vertices[i].vertex_tangent[0];
				meshToReturn.mesh_vertices[i].vertex_tangent[1] = vertices[i].vertex_tangent[1];
				meshToReturn.mesh_vertices[i].vertex_tangent[2] = vertices[i].vertex_tangent[2];
			}


			for (int i = 0; i < MAX_FILENAME; i++)
			{
				meshToReturn.mesh_meshID[i] = meshname.mesh_meshID[i];
			}

			

			//meshToReturn.mesh_meshID = meshname.mesh_meshID;
			//meshToReturn.mesh_materialID = meshname.mesh_materialID;

			customMeshFile.close();
			delete vertices;

		}

		return meshToReturn;
	}

	AnimatedMeshFromFile MyLibrary::Loadera::readAnimatedMeshFile(std::string fileName)
	{
		//reads the custom mesh file and stores all the data

		AnimatedMeshFromFile meshToReturn;

		bool fileIsOpen = false;

		std::ifstream customMeshFile(fileName, std::ifstream::binary);
		//customMeshFile.open(fileName, std::ifstream::binary);

		if (customMeshFile.is_open())
		{
			fileIsOpen = true;

			MeshHeader meshname;

			//customMeshFile.read((char*)&meshname.mesh_nrOfVertices, sizeof(meshname.mesh_nrOfVertices));
			//customMeshFile.read((char*)&meshname.mesh_meshID, sizeof(meshname.mesh_meshID));
			customMeshFile.read((char*)&meshname, sizeof(MeshHeader));

			meshToReturn.mesh_nrOfVertices = meshname.mesh_nrOfVertices;

			AnimatedVertex* vertices = new AnimatedVertex[meshname.mesh_nrOfVertices];
			customMeshFile.read((char*)vertices, meshname.mesh_nrOfVertices * sizeof(AnimatedVertex));

			meshToReturn.mesh_vertices = new AnimatedVertexFromFile[meshname.mesh_nrOfVertices];

			for (int i = 0; i < meshname.mesh_nrOfVertices; i++)
			{

				meshToReturn.mesh_vertices[i].vertex_position[0] = vertices[i].vertex_position[0];
				meshToReturn.mesh_vertices[i].vertex_position[1] = vertices[i].vertex_position[1];
				meshToReturn.mesh_vertices[i].vertex_position[2] = vertices[i].vertex_position[2];

				meshToReturn.mesh_vertices[i].vertex_UVCoord[0] = vertices[i].vertex_UVCoord[0];
				meshToReturn.mesh_vertices[i].vertex_UVCoord[1] = vertices[i].vertex_UVCoord[1];

				meshToReturn.mesh_vertices[i].vertex_normal[0] = vertices[i].vertex_normal[0];
				meshToReturn.mesh_vertices[i].vertex_normal[1] = vertices[i].vertex_normal[1];
				meshToReturn.mesh_vertices[i].vertex_normal[2] = vertices[i].vertex_normal[2];

				meshToReturn.mesh_vertices[i].influencing_joint[0] = vertices[i].influencing_joint[0];
				meshToReturn.mesh_vertices[i].influencing_joint[1] = vertices[i].influencing_joint[1];
				meshToReturn.mesh_vertices[i].influencing_joint[2] = vertices[i].influencing_joint[2];
				meshToReturn.mesh_vertices[i].influencing_joint[3] = vertices[i].influencing_joint[3];

				meshToReturn.mesh_vertices[i].joint_weights[0] = vertices[i].joint_weights[0];
				meshToReturn.mesh_vertices[i].joint_weights[1] = vertices[i].joint_weights[1];
				meshToReturn.mesh_vertices[i].joint_weights[2] = vertices[i].joint_weights[2];
				meshToReturn.mesh_vertices[i].joint_weights[3] = vertices[i].joint_weights[3];
			}


			for (int i = 0; i < MAX_FILENAME; i++)
			{
				meshToReturn.mesh_meshID[i] = meshname.mesh_meshID[i];
			}

			calculateTangentsAndBitangents(meshToReturn);

			//meshToReturn.mesh_meshID = meshname.mesh_meshID;
			//meshToReturn.mesh_materialID = meshname.mesh_materialID;

			customMeshFile.close();
		}

		return meshToReturn;
	}


	SkeletonFromFile Loadera::readSkeletonFile(std::string fileName)
	{
		//read the skeleton file

		SkeletonFromFile skeleton_to_return;

		bool fileIsOpen = false;

		std::ifstream customSkeletonFile(fileName, std::ifstream::binary);
		if (customSkeletonFile.is_open())
		{
			fileIsOpen = true;

			SkeletonHeader skeleton_header;
			customSkeletonFile.read((char*)&skeleton_header, sizeof(SkeletonHeader));


			Joint* joints = new Joint[skeleton_header.skeleton_nrOfJoints];

			//	skeleton_to_return.skeleton_joints = new Joint[skeleton_header.skeleton_nrOfJoints];

			customSkeletonFile.read((char*)joints, skeleton_header.skeleton_nrOfJoints * sizeof(Joint));
			skeleton_to_return.skeleton_joints = new Joint[skeleton_header.skeleton_nrOfJoints];
			for (int i = 0; i < skeleton_header.skeleton_nrOfJoints; i++)
			{
				for (int j = 0; j < MAX_FILENAME; j++)
					skeleton_to_return.skeleton_joints[i].joint_name[j] = joints[i].joint_name[j];
				skeleton_to_return.skeleton_joints[i].joint_transform = joints[i].joint_transform;

			}
			skeleton_to_return.skeleton_nrOfJoints = skeleton_header.skeleton_nrOfJoints;
			for (int i = 0; i < MAX_FILENAME; i++)
				skeleton_to_return.skeletonID[i] = skeleton_header.skeletonID[i];
			customSkeletonFile.close();
		}

		return skeleton_to_return;
	}

	AnimationFromFile Loadera::readAnimationFile(std::string fileName)
	{
		bool fileIsOpen = false;

		std::ifstream customAnimationFile(fileName, std::ifstream::binary);
		AnimationHeader animation_header;
		AnimationFromFile animation_to_return;

		if (customAnimationFile.is_open())
		{

			fileIsOpen = true;
			customAnimationFile.read((char*)&animation_header, sizeof(AnimationHeader));
			animation_to_return.nr_of_keyframes = animation_header.anim_nrOfKeys;
			Transform* keyframes = new Transform[animation_header.anim_nrOfKeys];
			customAnimationFile.read((char*)keyframes, animation_header.anim_nrOfKeys * sizeof(Transform));

			animation_to_return.keyframe_transformations = new Transform[animation_header.anim_nrOfKeys];
			for (int i = 0; i < animation_header.anim_nrOfKeys; i++)
			{
				animation_to_return.keyframe_transformations[i] = keyframes[i];
			}

			customAnimationFile.close();
		}

		return animation_to_return;
	}



	int Loadera::getNrOfVerticesFromFile(std::ifstream & file)
	{
		//read number of vertices in mesh from custom file
		int nrOfVerticesInFile = 0;


		return nrOfVerticesInFile;
	}

	//	void Loadera::calculateTangentsAndBitangents(MeshFromFile mesh)
	//	{
	//		for (int i = 0; i < mesh.mesh_nrOfVertices; i += 3)
	//		{
	//			//mesh.mesh_vertices[i].vertex_position;
	//			float vec1[3];
	//			float vec2[3];
	//
	//			vec1[0] = mesh.mesh_vertices[i + 1].vertex_position[0] - mesh.mesh_vertices[i].vertex_position[0];
	//			vec1[1] = mesh.mesh_vertices[i + 1].vertex_position[1] - mesh.mesh_vertices[i].vertex_position[1];
	//			vec1[2] = mesh.mesh_vertices[i + 1].vertex_position[2] - mesh.mesh_vertices[i].vertex_position[2];
	//
	//			vec2[0] = mesh.mesh_vertices[i + 2].vertex_position[0] - mesh.mesh_vertices[i].vertex_position[0];
	//			vec2[1] = mesh.mesh_vertices[i + 2].vertex_position[1] - mesh.mesh_vertices[i].vertex_position[1];
	//			vec2[2] = mesh.mesh_vertices[i + 2].vertex_position[2] - mesh.mesh_vertices[i].vertex_position[2];
	//
	//
	//			float uVec1[2];
	//			float uVec2[2];
	//
	//			uVec1[0] = mesh.mesh_vertices[i + 1].vertex_UVCoord[0] - mesh.mesh_vertices[i].vertex_UVCoord[0];
	//			uVec1[1] = mesh.mesh_vertices[i + 1].vertex_UVCoord[1] - mesh.mesh_vertices[i].vertex_UVCoord[1];
	//
	//			uVec2[0] = mesh.mesh_vertices[i + 2].vertex_UVCoord[0] - mesh.mesh_vertices[i].vertex_UVCoord[0];
	//			uVec2[1] = mesh.mesh_vertices[i + 2].vertex_UVCoord[1] - mesh.mesh_vertices[i].vertex_UVCoord[1];
	//
	//
	//			float denominator = (uVec1[0] * uVec2[1]) - (uVec1[1] * uVec2[0]);
	//			float someFloat = 1.0f / denominator;
	//
	//			float dVec1[3];
	//			float dVec2[3];
	//
	//			dVec1[0] = vec1[0] * uVec2[1];
	//			dVec1[1] = vec1[1] * uVec2[1];
	//			dVec1[2] = vec1[2] * uVec2[1];
	//
	//			dVec2[0] = vec2[0] * uVec1[1];
	//			dVec2[1] = vec2[1] * uVec1[1];
	//			dVec2[2] = vec2[2] * uVec1[1];
	//
	//			float tan[3];
	//			tan[0] = dVec1[0] - dVec2[0];
	//			tan[1] = dVec1[1] - dVec2[1];
	//			tan[2] = dVec1[2] - dVec2[2];
	//
	//			tan[0] = tan[0] * someFloat;
	//			tan[1] = tan[1] * someFloat;
	//			tan[2] = tan[2] * someFloat;
	//
	//
	//			//tangent			//store the tangent values 
	//			//DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&tan));			//normalize?
	//			mesh.mesh_vertices[i].vertex_tangent[0] = tan[0];
	//			mesh.mesh_vertices[i].vertex_tangent[1] = tan[1];
	//			mesh.mesh_vertices[i].vertex_tangent[2] = tan[2];
	//
	//			mesh.mesh_vertices[i + 1].vertex_tangent[0] = tan[0];
	//			mesh.mesh_vertices[i + 1].vertex_tangent[1] = tan[1];
	//			mesh.mesh_vertices[i + 1].vertex_tangent[2] = tan[2];
	//
	//			mesh.mesh_vertices[i + 2].vertex_tangent[0] = tan[0];
	//			mesh.mesh_vertices[i + 2].vertex_tangent[1] = tan[1];
	//			mesh.mesh_vertices[i + 2].vertex_tangent[2] = tan[2];
	//
	//			//bitangent
	//			//DirectX::XMVECTOR bitangent = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&vertex[i].tangent), DirectX::XMLoadFloat3(&vertex[i].normal)));
	//			float bitangent[3];
	//			bitangent[0] = (mesh.mesh_vertices[i].vertex_tangent[1] * mesh.mesh_vertices[i].vertex_normal[2] - mesh.mesh_vertices[i].vertex_tangent[2] * mesh.mesh_vertices[i].vertex_normal[1]);
	//			bitangent[1] = (mesh.mesh_vertices[i].vertex_tangent[2] * mesh.mesh_vertices[i].vertex_normal[0] - mesh.mesh_vertices[i].vertex_tangent[0] * mesh.mesh_vertices[i].vertex_normal[2]);
	//			bitangent[2] = (mesh.mesh_vertices[i].vertex_tangent[0] * mesh.mesh_vertices[i].vertex_normal[1] - mesh.mesh_vertices[i].vertex_tangent[1] * mesh.mesh_vertices[i].vertex_normal[0]);
	//
	//			//bitangent[0] = (mesh.mesh_vertices[i + 1].vertex_tangent[1] * mesh.mesh_vertices[i + 1].vertex_normal[2] - mesh.mesh_vertices[i + 1].vertex_tangent[2] * mesh.mesh_vertices[i + 1].vertex_normal[1]);
	//			//bitangent[1] = (mesh.mesh_vertices[i + 1].vertex_tangent[2] * mesh.mesh_vertices[i + 1].vertex_normal[0] - mesh.mesh_vertices[i + 1].vertex_tangent[0] * mesh.mesh_vertices[i + 1].vertex_normal[2]);
	//			//bitangent[2] = (mesh.mesh_vertices[i + 1].vertex_tangent[0] * mesh.mesh_vertices[i + 1].vertex_normal[1] - mesh.mesh_vertices[i + 1].vertex_tangent[1] * mesh.mesh_vertices[i + 1].vertex_normal[0]);
	//			//								   
	//			//bitangent[0] = (mesh.mesh_vertices[i + 2].vertex_tangent[1] * mesh.mesh_vertices[i + 2].vertex_normal[2] - mesh.mesh_vertices[i + 2].vertex_tangent[2] * mesh.mesh_vertices[i + 2].vertex_normal[1]);
	//			//bitangent[1] = (mesh.mesh_vertices[i + 2].vertex_tangent[2] * mesh.mesh_vertices[i + 2].vertex_normal[0] - mesh.mesh_vertices[i + 2].vertex_tangent[0] * mesh.mesh_vertices[i + 2].vertex_normal[2]);
	//			//bitangent[2] = (mesh.mesh_vertices[i + 2].vertex_tangent[0] * mesh.mesh_vertices[i + 2].vertex_normal[1] - mesh.mesh_vertices[i + 2].vertex_tangent[1] * mesh.mesh_vertices[i + 2].vertex_normal[0]);
	//
	//			mesh.mesh_vertices[i].vertex_biTangent[0] = bitangent[0];
	//			mesh.mesh_vertices[i].vertex_biTangent[1] = bitangent[1];
	//			mesh.mesh_vertices[i].vertex_biTangent[2] = bitangent[2];
	//
	//			mesh.mesh_vertices[i + 1].vertex_biTangent[0] = bitangent[0];
	//			mesh.mesh_vertices[i + 1].vertex_biTangent[1] = bitangent[1];
	//			mesh.mesh_vertices[i + 1].vertex_biTangent[2] = bitangent[2];
	//
	//			mesh.mesh_vertices[i + 2].vertex_biTangent[0] = bitangent[0];
	//			mesh.mesh_vertices[i + 2].vertex_biTangent[1] = bitangent[1];
	//			mesh.mesh_vertices[i + 2].vertex_biTangent[2] = bitangent[2];
	//		}
	//	}
	//}
	template<class T>
	inline void MyLibrary::Loadera::calculateTangentsAndBitangents(T mesh)
	{
		for (int i = 0; i < mesh.mesh_nrOfVertices; i += 3)
		{
			//mesh.mesh_vertices[i].vertex_position;
			float vec1[3];
			float vec2[3];

			vec1[0] = mesh.mesh_vertices[i + 1].vertex_position[0] - mesh.mesh_vertices[i].vertex_position[0];
			vec1[1] = mesh.mesh_vertices[i + 1].vertex_position[1] - mesh.mesh_vertices[i].vertex_position[1];
			vec1[2] = mesh.mesh_vertices[i + 1].vertex_position[2] - mesh.mesh_vertices[i].vertex_position[2];

			vec2[0] = mesh.mesh_vertices[i + 2].vertex_position[0] - mesh.mesh_vertices[i].vertex_position[0];
			vec2[1] = mesh.mesh_vertices[i + 2].vertex_position[1] - mesh.mesh_vertices[i].vertex_position[1];
			vec2[2] = mesh.mesh_vertices[i + 2].vertex_position[2] - mesh.mesh_vertices[i].vertex_position[2];


			float uVec1[2];
			float uVec2[2];

			uVec1[0] = mesh.mesh_vertices[i + 1].vertex_UVCoord[0] - mesh.mesh_vertices[i].vertex_UVCoord[0];
			uVec1[1] = mesh.mesh_vertices[i + 1].vertex_UVCoord[1] - mesh.mesh_vertices[i].vertex_UVCoord[1];

			uVec2[0] = mesh.mesh_vertices[i + 2].vertex_UVCoord[0] - mesh.mesh_vertices[i].vertex_UVCoord[0];
			uVec2[1] = mesh.mesh_vertices[i + 2].vertex_UVCoord[1] - mesh.mesh_vertices[i].vertex_UVCoord[1];


			float denominator = (uVec1[0] * uVec2[1]) - (uVec1[1] * uVec2[0]);
			float someFloat = 1.0f / denominator;

			float dVec1[3];
			float dVec2[3];

			dVec1[0] = vec1[0] * uVec2[1];
			dVec1[1] = vec1[1] * uVec2[1];
			dVec1[2] = vec1[2] * uVec2[1];

			dVec2[0] = vec2[0] * uVec1[1];
			dVec2[1] = vec2[1] * uVec1[1];
			dVec2[2] = vec2[2] * uVec1[1];

			float tan[3];
			tan[0] = dVec1[0] - dVec2[0];
			tan[1] = dVec1[1] - dVec2[1];
			tan[2] = dVec1[2] - dVec2[2];

			tan[0] = tan[0] * someFloat;
			tan[1] = tan[1] * someFloat;
			tan[2] = tan[2] * someFloat;


			//tangent			//store the tangent values 
			//DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&tan));			//normalize?
			mesh.mesh_vertices[i].vertex_tangent[0] = tan[0];
			mesh.mesh_vertices[i].vertex_tangent[1] = tan[1];
			mesh.mesh_vertices[i].vertex_tangent[2] = tan[2];

			mesh.mesh_vertices[i + 1].vertex_tangent[0] = tan[0];
			mesh.mesh_vertices[i + 1].vertex_tangent[1] = tan[1];
			mesh.mesh_vertices[i + 1].vertex_tangent[2] = tan[2];

			mesh.mesh_vertices[i + 2].vertex_tangent[0] = tan[0];
			mesh.mesh_vertices[i + 2].vertex_tangent[1] = tan[1];
			mesh.mesh_vertices[i + 2].vertex_tangent[2] = tan[2];

			//bitangent
			//DirectX::XMVECTOR bitangent = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&vertex[i].tangent), DirectX::XMLoadFloat3(&vertex[i].normal)));
			float bitangent[3];
			bitangent[0] = (mesh.mesh_vertices[i].vertex_tangent[1] * mesh.mesh_vertices[i].vertex_normal[2] - mesh.mesh_vertices[i].vertex_tangent[2] * mesh.mesh_vertices[i].vertex_normal[1]);
			bitangent[1] = (mesh.mesh_vertices[i].vertex_tangent[2] * mesh.mesh_vertices[i].vertex_normal[0] - mesh.mesh_vertices[i].vertex_tangent[0] * mesh.mesh_vertices[i].vertex_normal[2]);
			bitangent[2] = (mesh.mesh_vertices[i].vertex_tangent[0] * mesh.mesh_vertices[i].vertex_normal[1] - mesh.mesh_vertices[i].vertex_tangent[1] * mesh.mesh_vertices[i].vertex_normal[0]);

			//bitangent[0] = (mesh.mesh_vertices[i + 1].vertex_tangent[1] * mesh.mesh_vertices[i + 1].vertex_normal[2] - mesh.mesh_vertices[i + 1].vertex_tangent[2] * mesh.mesh_vertices[i + 1].vertex_normal[1]);
			//bitangent[1] = (mesh.mesh_vertices[i + 1].vertex_tangent[2] * mesh.mesh_vertices[i + 1].vertex_normal[0] - mesh.mesh_vertices[i + 1].vertex_tangent[0] * mesh.mesh_vertices[i + 1].vertex_normal[2]);
			//bitangent[2] = (mesh.mesh_vertices[i + 1].vertex_tangent[0] * mesh.mesh_vertices[i + 1].vertex_normal[1] - mesh.mesh_vertices[i + 1].vertex_tangent[1] * mesh.mesh_vertices[i + 1].vertex_normal[0]);
			//								   
			//bitangent[0] = (mesh.mesh_vertices[i + 2].vertex_tangent[1] * mesh.mesh_vertices[i + 2].vertex_normal[2] - mesh.mesh_vertices[i + 2].vertex_tangent[2] * mesh.mesh_vertices[i + 2].vertex_normal[1]);
			//bitangent[1] = (mesh.mesh_vertices[i + 2].vertex_tangent[2] * mesh.mesh_vertices[i + 2].vertex_normal[0] - mesh.mesh_vertices[i + 2].vertex_tangent[0] * mesh.mesh_vertices[i + 2].vertex_normal[2]);
			//bitangent[2] = (mesh.mesh_vertices[i + 2].vertex_tangent[0] * mesh.mesh_vertices[i + 2].vertex_normal[1] - mesh.mesh_vertices[i + 2].vertex_tangent[1] * mesh.mesh_vertices[i + 2].vertex_normal[0]);

			mesh.mesh_vertices[i].vertex_biTangent[0] = bitangent[0];
			mesh.mesh_vertices[i].vertex_biTangent[1] = bitangent[1];
			mesh.mesh_vertices[i].vertex_biTangent[2] = bitangent[2];

			mesh.mesh_vertices[i + 1].vertex_biTangent[0] = bitangent[0];
			mesh.mesh_vertices[i + 1].vertex_biTangent[1] = bitangent[1];
			mesh.mesh_vertices[i + 1].vertex_biTangent[2] = bitangent[2];

			mesh.mesh_vertices[i + 2].vertex_biTangent[0] = bitangent[0];
			mesh.mesh_vertices[i + 2].vertex_biTangent[1] = bitangent[1];
			mesh.mesh_vertices[i + 2].vertex_biTangent[2] = bitangent[2];
		}
	}
}