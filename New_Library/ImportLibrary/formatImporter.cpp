#include "ImporterPCH.h"
#include "formatImporter.h"

namespace ImporterLibrary
{
#pragma region ImportHelpers
	Vec4 CustomFileLoader::loadVec4(std::ifstream& file)
	{
		if (!file.is_open())
			return Vec4();

		Vec4 vec;
		file.read((char*)&vec, sizeof(Vec4));

		return vec;
	}

	// Reads a 32-bit int
	int32_t CustomFileLoader::loadInt32(std::ifstream& file)
	{
		if (!file.is_open())
			return 0;

		int32_t int32;
		file.read((char*)&int32, sizeof(int32_t));
		return int32;
	}

	// Reads a DecomposedTransform
	// (containing 3xVec4 (T, R, S))
	DecomposedTransform CustomFileLoader::loadTransform(std::ifstream& file)
	{
		if (!file.is_open())
			return DecomposedTransform();

		DecomposedTransform transform;

		transform.translation = (loadVec4(file));
		transform.translation.z *= -1.0f;

		transform.rotation = (loadVec4(file));
		transform.rotation.x *= -1.0f;
		transform.rotation.y *= -1.0f;

		transform.scale = (loadVec4(file));

		return transform;
	}

	// Reads a Bone 
	// (containing 2xDecomposedTransform and one 32-bit wide int)
	Bone CustomFileLoader::loadBone(std::ifstream& file)
	{
		if (!file.is_open())
			return Bone();

		Bone bone;
		bone.jointInverseBindPoseTransform = loadTransform(file);
		bone.jointReferenceTransform = loadTransform(file);
		bone.parentIndex = loadInt32(file);

		return bone;
	}

	// Reads a Skeleton
	// (Containing boneCount x Bone)
	Skeleton CustomFileLoader::loadSkeleton(std::ifstream& file, int32_t boneCount)
	{
		if (!file.is_open())
			return Skeleton();

		/// Read and add each bone to vector
		Skeleton skeleton;
		for (int i = 0; i < boneCount; i++)
			skeleton.joints.push_back(loadBone(file));

		return skeleton;
	}

	// Reads a Skeleton
	// (Containing boneCount x Bone)
	// No bonecount is supplied; we assume we read that data first (one 32-bit wide int)
	Skeleton CustomFileLoader::loadSkeleton(std::ifstream& file)
	{
		if (!file.is_open())
			return Skeleton();

		/// Read bone count
		int32_t boneCount = loadInt32(file);

		/// Read and add each bone to vector
		Skeleton skeleton;
		for (int i = 0; i < boneCount; i++)
			skeleton.joints.push_back(loadBone(file));

		return skeleton;
	}
#pragma endregion ImportHelpers

	CustomFileLoader::CustomFileLoader()
	{

	}

	CustomFileLoader::~CustomFileLoader()
	{
	}
	PropItemToEngine CustomFileLoader::readPropsFile(const std::string & fileName)
	{
		std::string newFileName = "../Assets/";
		newFileName.append(fileName + "FOLDER/" + fileName + "_PROPS.bin");

		PropItemToEngine toReturn;
		propsHeader header;
		bool fileIsOpen = false;

		std::ifstream customPropFile(newFileName, std::ifstream::binary);

		if (customPropFile.is_open())
		{
			customPropFile.read((char*)&header.nrOfItems, sizeof(int));
			toReturn.nrOfItems = header.nrOfItems;
			toReturn.props = new PropItem[toReturn.nrOfItems];

			//CollisionBox* box = new CollisionBox[collisionBoxes.nrOfBoxes]; // i guess i nee
			//customBoxFile.read((char*)box, collisionBoxes.nrOfBoxes * sizeof(CollisionBox));

			for (int i = 0; i < header.nrOfItems; i++)
			{
				customPropFile.read((char*)&toReturn.props[i].typeOfProp, sizeof(int));
				customPropFile.read((char*)&toReturn.props[i].linkedItem, sizeof(int));
				customPropFile.read((char*)&toReturn.props[i].isTrigger, sizeof(bool));
														 
				customPropFile.read((char*)&toReturn.props[i].transform_position, sizeof(float) * 3);
				customPropFile.read((char*)&toReturn.props[i].transform_rotation, sizeof(float) * 3);
				customPropFile.read((char*)&toReturn.props[i].transform_scale, sizeof(float) * 3);
				customPropFile.read((char*)&toReturn.props[i].BBOX_INFO, sizeof(float) * 3);
				
				

			}
			//customPropFile.read((char*)toReturn.props, sizeof(PropItem) * (toReturn.nrOfItems));
		}
		customPropFile.close();
		return toReturn;
	}
	MeshFromFile CustomFileLoader::readMeshFile(std::string fileName)
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
			delete[] vertices;

		}

		return meshToReturn;
	}

	AnimatedMeshFromFile ImporterLibrary::CustomFileLoader::readAnimatedMeshFile(std::string fileName)
	{
		//reads the custom mesh file and stores all the data

		AnimatedMeshFromFile meshToReturn = {};

		bool fileIsOpen = false;

		std::ifstream customMeshFile(fileName, std::ifstream::binary);
		//customMeshFile.open(fileName, std::ifstream::binary);

		if (customMeshFile.is_open())
		{
			fileIsOpen = true;

			MeshHeader meshname = {};

			//customMeshFile.read((char*)&meshname.mesh_nrOfVertices, sizeof(meshname.mesh_nrOfVertices));
			//customMeshFile.read((char*)&meshname.mesh_meshID, sizeof(meshname.mesh_meshID));
			customMeshFile.read((char*)&meshname, sizeof(MeshHeader));

			meshToReturn.mesh_nrOfVertices = meshname.mesh_nrOfVertices;

			AnimatedVertex* vertices = new AnimatedVertex[meshname.mesh_nrOfVertices];
			customMeshFile.read((char*)vertices, meshname.mesh_nrOfVertices * sizeof(AnimatedVertex));

			meshToReturn.mesh_vertices = new AnimatedVertexFromFile[meshname.mesh_nrOfVertices];
	
			for (unsigned int i = 0; i < meshname.mesh_nrOfVertices; i++)
			{
				meshToReturn.mesh_vertices[i] = AnimatedVertexFromFile();
				meshToReturn.mesh_vertices[i].vertex_position[0] = vertices[i].vertex_position[0];
				meshToReturn.mesh_vertices[i].vertex_position[1] = vertices[i].vertex_position[1];
				meshToReturn.mesh_vertices[i].vertex_position[2] = -vertices[i].vertex_position[2];

				meshToReturn.mesh_vertices[i].vertex_UVCoord[0] = vertices[i].vertex_UVCoord[0];
				meshToReturn.mesh_vertices[i].vertex_UVCoord[1] = vertices[i].vertex_UVCoord[1];

				meshToReturn.mesh_vertices[i].vertex_normal[0] = vertices[i].vertex_normal[0];
				meshToReturn.mesh_vertices[i].vertex_normal[1] = vertices[i].vertex_normal[1];
				meshToReturn.mesh_vertices[i].vertex_normal[2] = -vertices[i].vertex_normal[2];

				meshToReturn.mesh_vertices[i].vertex_tangent[0] = vertices[i].vertex_tangent[0];
				meshToReturn.mesh_vertices[i].vertex_tangent[1] = vertices[i].vertex_tangent[1];
				meshToReturn.mesh_vertices[i].vertex_tangent[2] =- vertices[i].vertex_tangent[2];

				meshToReturn.mesh_vertices[i].influencing_joint[0] = vertices[i].influencing_joint[0];
				meshToReturn.mesh_vertices[i].influencing_joint[1] = vertices[i].influencing_joint[1];
				meshToReturn.mesh_vertices[i].influencing_joint[2] = vertices[i].influencing_joint[2];
				meshToReturn.mesh_vertices[i].influencing_joint[3] = vertices[i].influencing_joint[3];

				meshToReturn.mesh_vertices[i].joint_weights[0] = vertices[i].joint_weights[0];
				meshToReturn.mesh_vertices[i].joint_weights[1] = vertices[i].joint_weights[1];
				meshToReturn.mesh_vertices[i].joint_weights[2] = vertices[i].joint_weights[2];
				meshToReturn.mesh_vertices[i].joint_weights[3] = vertices[i].joint_weights[3];
			}

			// Switch winding order
			/*for (unsigned int i = 0; i < meshname.mesh_nrOfVertices; i += 3)
				std::swap(meshToReturn.mesh_vertices[i + 1], meshToReturn.mesh_vertices[i + 2]);*/

			for (int i = 0; i < MAX_FILENAME; i++)
			{
				meshToReturn.mesh_meshID[i] = meshname.mesh_meshID[i];
			}

			customMeshFile.close();
			delete[] vertices;
		}

		return meshToReturn;
	}

	Skeleton CustomFileLoader::readSkeletonFile(std::string fileName)
	{
		//read the skeleton file

		Skeleton skeleton_to_return = {};

		std::ifstream customSkeletonFile(fileName, std::ifstream::binary);

		assert(customSkeletonFile.is_open());

		if (customSkeletonFile.is_open())
		{
			int32_t jointCount = loadInt32(customSkeletonFile);
			skeleton_to_return = loadSkeleton(customSkeletonFile, jointCount);
		}
		

		return skeleton_to_return;
	}

	ImporterLibrary::AnimationFromFileStefan CustomFileLoader::readAnimationFile(std::string fileName, uint16_t jointCount)
	{
		std::ifstream customAnimationFile(fileName, std::ifstream::binary);
		assert(customAnimationFile.is_open());

		AnimationFromFileStefan animation_to_return;

		if (customAnimationFile.is_open())
		{
			uint32_t numberOfKeys = loadInt32(customAnimationFile);
			animation_to_return.nr_of_keyframes = numberOfKeys;

			DecomposedTransform* keyframes = new DecomposedTransform[numberOfKeys * jointCount];

			//Init keyframes
			for (unsigned int i = 0; i < animation_to_return.nr_of_keyframes * jointCount; i++)
				keyframes[i] = DecomposedTransform();

			for (unsigned int i = 0; i < animation_to_return.nr_of_keyframes * jointCount; i++)
				keyframes[i] = loadTransform(customAnimationFile);

			animation_to_return.keyframe_transformations = std::make_unique<DecomposedTransform[]>(numberOfKeys * jointCount);
			for (unsigned int i = 0; i < numberOfKeys * jointCount; i++)
			{
				animation_to_return.keyframe_transformations[i] = keyframes[i];
			}

			delete[] keyframes;
			customAnimationFile.close();
		}

		return animation_to_return;
	}
	CollisionBoxes CustomFileLoader::readMeshCollisionBoxes(const std::string & fileName)
	{
		CollisionBoxes collisionBoxes;

		bool fileIsOpen = false;

		std::ifstream customBoxFile(fileName, std::ifstream::binary);
	

		if (customBoxFile.is_open()) // opens file
		{
			fileIsOpen = true; // ya its open

			CollisionHeader header; // okey i guess i need an error
			customBoxFile.read((char*)&header, sizeof(CollisionHeader)); // what is happening here?!! i just copied this code

			collisionBoxes.nrOfBoxes = header.nrOfBoxes; // get the number of boxes

			CollisionBox* box = new CollisionBox[collisionBoxes.nrOfBoxes]; // i guess i need an array
			customBoxFile.read((char*)box, collisionBoxes.nrOfBoxes * sizeof(CollisionBox)); // got litraly no fucking clue whats going on here please send help

			collisionBoxes.boxes = new CollisionBox[collisionBoxes.nrOfBoxes]; // why can't i use the first array

			for (unsigned int i = 0; i < collisionBoxes.nrOfBoxes; i++)
			{
				//Do the copy thing	
				collisionBoxes.boxes[i].translation[0] = box[i].translation[0];
				collisionBoxes.boxes[i].translation[1] = box[i].translation[1];
				collisionBoxes.boxes[i].translation[2] = box[i].translation[2];

				collisionBoxes.boxes[i].scale[0] = box[i].scale[0];
				collisionBoxes.boxes[i].scale[1] = box[i].scale[1];
				collisionBoxes.boxes[i].scale[2] = box[i].scale[2];

				collisionBoxes.boxes[i].rotation[0] = box[i].rotation[0];
				collisionBoxes.boxes[i].rotation[1] = box[i].rotation[1];
				collisionBoxes.boxes[i].rotation[2] = box[i].rotation[2];
				collisionBoxes.boxes[i].rotation[3] = box[i].rotation[3];

				collisionBoxes.boxes[i].typeOfBox = box[i].typeOfBox;
				//std::cout << ":..............................." << std::endl;
				//std::cout << "BoX T:" << collisionBoxes.boxes[i].translation[0] << std::endl;
				//std::cout << "BoX T:" << collisionBoxes.boxes[i].translation[1] << std::endl;
				//std::cout << "BoX T:" << collisionBoxes.boxes[i].translation[2] << std::endl;
				//std::cout << "BoX R:" << collisionBoxes.boxes[i].rotation[0] << std::endl;
				//std::cout << "BoX R:" << collisionBoxes.boxes[i].rotation[1] << std::endl;
				//std::cout << "BoX R:" << collisionBoxes.boxes[i].rotation[2] << std::endl;
				//std::cout << "BoX R:" << collisionBoxes.boxes[i].rotation[3] << std::endl;
				//std::cout << "BoX S:" << collisionBoxes.boxes[i].scale[0] << std::endl;
				//std::cout << "BoX S:" << collisionBoxes.boxes[i].scale[1] << std::endl;
				//std::cout << "BoX S:" << collisionBoxes.boxes[i].scale[2] << std::endl;
				//std::cout << "Type of box: " << collisionBoxes.boxes[i].typeOfBox << std::endl;
				//std::cout << ":..............................." << std::endl;

			}
			//copy thing done

			customBoxFile.close(); // close file
			delete[] box; // gotta keep ya memmory happy

		}
		else
			return CollisionBoxes();
		return collisionBoxes; // done :D
	}
	PointLights CustomFileLoader::readLightFile(const std::string & fileName)
	{
		PointLights pointLights;
		bool fileIsOpen = false;

		std::string newFileName = "../Assets/";
		newFileName.append(fileName + "FOLDER/" + fileName + "_LIGHTS.bin");

		std::ifstream customLightFile(newFileName, std::ifstream::binary);

		if (customLightFile.is_open()) // opens file
		{
			fileIsOpen = true; // ya its open
			int nrOf;

			customLightFile.read((char*)&nrOf, sizeof(int)); // what is happening here?!! i just copied this code
			pointLights.nrOf = nrOf;
			pointLights.lights = new pointLight[nrOf];
			customLightFile.read((char*)pointLights.lights, pointLights.nrOf * sizeof(pointLight)); // got litraly no fucking clue whats going on here please send help

			//copy thing done
			customLightFile.close(); // close file
		}
		else
			return PointLights();
		return pointLights; // done :D
	}


	StartingPos CustomFileLoader::readPlayerStartFile(const std::string & fileName, int whichPlayer)
	{
		StartingPos startPos;
		bool fileIsOpen = false;

		
		std::string newFileName = "../Assets/";
		newFileName.append(fileName + "FOLDER/" + fileName + "_PPOS" + to_string(whichPlayer)  + ".bin");
		std::ifstream customPosFile(newFileName, std::ifstream::binary);
		if (customPosFile.is_open()) // opens file
		{
			fileIsOpen = true; // ya its open

			customPosFile.read((char*)&startPos, sizeof(StartingPos)); // what is happening here?!! i just copied this code
			
			//copy thing done
			customPosFile.close(); // close file
		}
		else
			return StartingPos();
		return startPos; // done :D
	}
	GuardStartingPositions CustomFileLoader::readGuardStartFiles(const std::string & fileName)
	{
		GuardStartingPositions guardPos;

		bool fileIsOpen = false;
		std::string newFileName = "../Assets/";
		newFileName.append(fileName + "FOLDER/" + fileName + "_GPOS.bin");

		std::ifstream customGuardFile(newFileName, std::ifstream::binary);


		if (customGuardFile.is_open()) // opens file
		{
			fileIsOpen = true; // ya its open

			
			customGuardFile.read((char*)&guardPos.nrOf, sizeof(int)); // what is happening here?!! i just copied this code

			guardPos.startingPositions = new StartingPos[guardPos.nrOf];
			customGuardFile.read((char*)guardPos.startingPositions, sizeof(StartingPos) * guardPos.nrOf); // what is happening here?!! i just copied this code

			
			//copy thing done

			customGuardFile.close(); // close file
			

		}
		else
			return GuardStartingPositions();
		return guardPos; // done :D
	}
	GridStruct * CustomFileLoader::readGridFile(const std::string & fileName)
	{
		GridStruct * gridPos = new GridStruct();
		std::string newFileName = "../Assets/";
		newFileName.append(fileName + "FOLDER/" + fileName + "_GRID.bin");
		std::ifstream customGridFile(newFileName, std::ifstream::binary);
		
		if (customGridFile.is_open())
		{
		

			customGridFile.read((char*)&gridPos->maxX, sizeof(int)); // what is happening here?!! i just copied this code
			customGridFile.read((char*)&gridPos->maxY, sizeof(int)); // what is happening here?!! i just copied this code

			customGridFile.read((char*)&gridPos->nrOf, sizeof(int)); // what is happening here?!! i just copied this code

			gridPos->gridPoints = new GridPointStruct[gridPos->nrOf];
			
			for (int i = 0; i < gridPos->nrOf; i++)
			{
				customGridFile.read((char*)&gridPos->gridPoints[i].pathable, sizeof(bool)); // what is happening here?!! i just copied this code
				customGridFile.read((char*)&gridPos->gridPoints[i].translation, sizeof(float)*3); // what is happening here?!! i just copied this code
			}
			customGridFile.close();
		}
		else
			return new GridStruct();
		return gridPos;
	}
}