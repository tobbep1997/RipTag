#pragma once
#include <vector>
#include "../../Structs.h"
#pragma comment(lib, "New_Library.lib")
#include <../New_Library/formatImporter.h>
#include <../Engine/Source/3D Engine/Model/AnimatedModel.h>
class DynamicMesh 
{
private:
	std::vector <DynamicVertex> m_dynamicVertex;
	std::string m_meshName;
public:
	DynamicMesh();
	~DynamicMesh();
	Animation::AnimatedModel *anim;
	const DynamicVertex * getRawVertices() const;
	const std::vector<DynamicVertex> & getVertices() const;

	void setVertices(std::vector<DynamicVertex>& input);

	void SET_DEFAULT();

	void LoadMesh(const std::string & path);
};

