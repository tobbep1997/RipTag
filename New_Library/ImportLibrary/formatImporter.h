#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "FormatHeader.h"

namespace ImporterLibrary
{
	template <typename T> std::string to_string(const T& n)
	{
		std::ostringstream stream;
		stream << n;
		return stream.str();
	}

	class CustomFileLoader
	{
	public:
		CustomFileLoader();
		~CustomFileLoader();

		MeshFromFile readMeshFile(std::string fileName);
		SkinnedMeshFromFile readSkinnedMeshFile(std::string fileName);
		//SkeletonFromFile readSkeletonFile(std::string fileName);
		ImporterLibrary::Skeleton readSkeletonFile(std::string fileName);
		//AnimationFromFile readAnimationFile(std::string fileName, uint16_t jointCount);
		AnimationFromFileStefan readAnimationFile(std::string fileName, uint16_t jointCount);

		CollisionBoxes readMeshCollisionBoxes(const std::string & fileName);
		PointLights readLightFile(const std::string & roomIndex);

		StartingPos readPlayerStartFile(const std::string & fileName, int whichPlayer);
		GuardStartingPositions readGuardStartFiles(const std::string & fileName);
		GridStruct * readGridFile(const std::string & fileName);
		PropItemToEngine readPropsFile(const std::string & fileName);

		reverbPointToEngine readReverbPointFile(const std::string & filename);
		SoundPointToEngine readSoundPointFile(const std::string & filename);



	private:
		Skeleton loadSkeleton(std::ifstream& file);
		Skeleton loadSkeleton(std::ifstream& file, int32_t boneCount);
		Vec4 loadVec4(std::ifstream& file);
		int32_t loadInt32(std::ifstream& file);
		DecomposedTransform loadTransform(std::ifstream& file);
		Bone loadBone(std::ifstream& file);		
	};

}