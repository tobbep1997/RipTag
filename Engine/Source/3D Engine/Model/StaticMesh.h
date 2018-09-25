#pragma once
#include <vector>
#include "../../Structs.h"
#pragma comment(lib, "New_Library.lib")
//#include <../../../New_Library/formatImporter.h>

#include <../New_Library/formatImporter.h>

class StaticMesh
{
private:
	std::string m_meshName;
	std::vector<StaticVertex> m_staticVertex;
public:
	StaticMesh();
	~StaticMesh();

	const StaticVertex * getRawVertice() const;
	const std::vector<StaticVertex> & getVertice() const;
	void setVertices(std::vector<StaticVertex>& input);

	void SET_DEFAULT();

	void LoadMesh(const std::string & path);

};

