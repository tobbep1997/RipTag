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

			for (unsigned int i = 0; i < meshname.mesh_nrOfVertices; i++)
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
	
			for (unsigned int i = 0; i < meshname.mesh_nrOfVertices; i++)
			{

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

			customMeshFile.close();
			delete vertices;
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
			for (unsigned int i = 0; i < skeleton_header.skeleton_nrOfJoints; i++)
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
			for (unsigned int i = 0; i < animation_header.anim_nrOfKeys; i++)
			{
				animation_to_return.keyframe_transformations[i] = keyframes[i];
			}

			customAnimationFile.close();
		}

		return animation_to_return;
	}



	int Loadera::getNrOfVerticesFromFile(std::ifstream & file)
	{
		int nrOfVerticesInFile = 0;
		return nrOfVerticesInFile;
	}

}