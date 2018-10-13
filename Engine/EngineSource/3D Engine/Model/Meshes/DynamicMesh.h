#pragma once
#include <vector>
#include "../../../Structs.h"
#pragma comment(lib, "New_Library.lib")
#include "ImportLibrary/formatImporter.h"
#include "AnimatedModel.h"
class DynamicMesh 
{
private:
	std::vector <DynamicVertex> m_dynamicVertex;
	std::string m_meshName;
public:
	DynamicMesh();
	~DynamicMesh();
	Animation::AnimatedModel *m_anim;
	const DynamicVertex * getRawVertices() const;
	const std::vector<DynamicVertex> & getVertices() const;
	Animation::AnimatedModel* getAnimatedModel();
	void setVertices(std::vector<DynamicVertex>& input);

	void SET_DEFAULT();

	void setName(const std::string & name);
	const std::string & getName() const;

	void LoadMesh(const std::string & path);
};

