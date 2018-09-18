#pragma once
#include <vector>
#include "../../Structs.h"
#pragma comment(lib, "New_Library.lib")
#include <../New_Library/formatImporter.h>
#include <../Engine/Source/3D Engine/Model/AnimatedModel.h>
class DynamicMesh 
{
private:
	std::vector <DynamicVertex> dynamicVertex;
	
public:
	DynamicMesh();
	~DynamicMesh();
	Animation::AnimatedModel *anim;
	const DynamicVertex * getRawVertices() const;
	const std::vector<DynamicVertex> & getVertices() const;
	Animation::AnimatedModel* getAnimatedModel();
	void setVertices(std::vector<DynamicVertex>& input);

	void SET_DEFAULT();

	void LoadModel(const std::string & path);
};

