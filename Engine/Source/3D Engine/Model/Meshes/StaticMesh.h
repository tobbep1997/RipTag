#pragma once
#include <vector>
#include "../../../Structs.h"
//#include <../../../New_Library/formatImporter.h>
#pragma comment(lib, "New_Library")
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

	void setName(const std::string & name);
	const std::string & getName() const;

	void LoadMesh(const std::string & path);

};
