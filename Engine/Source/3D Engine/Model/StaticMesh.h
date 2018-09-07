#pragma once
#include <vector>
#include "../../Structs.h"
#pragma comment(lib, "../../../../x64/Debug/New_Library.lib")
#include <../../../New_Library/formatImporter.h>
class StaticMesh
{
private:
	std::vector<StaticVertex> staticVertex;

public:
	StaticMesh();
	~StaticMesh();

	const StaticVertex * getRawVertice() const;
	const std::vector<StaticVertex> & getVertice() const;

	void setVertices(std::vector<StaticVertex>& input);

	void SET_DEFAULT();

};

