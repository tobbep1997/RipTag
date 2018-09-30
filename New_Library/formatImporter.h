#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "FormatHeader.h"

namespace MyLibrary
{
	class Loadera
	{
	public:
		Loadera();
		~Loadera();

		MeshFromFile readMeshFile(std::string fileName);
		AnimatedMeshFromFile readAnimatedMeshFile(std::string fileName);
		SkeletonFromFile readSkeletonFile(std::string fileName);
		MyLibrary::Skeleton readSkeletonFileStefan(std::string fileName);
		AnimationFromFile readAnimationFile(std::string fileName, uint16_t jointCount);
		AnimationFromFileStefan readAnimationFileStefan(std::string fileName, uint16_t jointCount);
	private:
		Skeleton loadSkeleton(std::ifstream& file);
		Skeleton loadSkeleton(std::ifstream& file, int32_t boneCount);
		Vec4 loadVec4(std::ifstream& file);
		int32_t loadInt32(std::ifstream& file);
		DecomposedTransform loadTransform(std::ifstream& file);
		Bone loadBone(std::ifstream& file);
	private:
		int getNrOfVerticesFromFile(std::ifstream& file);
		
	};

}