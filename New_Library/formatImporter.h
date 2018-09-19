#pragma once
#include <vector>
#include <string>
#include <fstream>
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
		AnimationFromFile readAnimationFile(std::string fileName, uint16_t jointCount);
		
	private:
		int getNrOfVerticesFromFile(std::ifstream& file);
		
	};

}