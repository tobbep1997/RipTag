#pragma once
#include <vector>
#include "../../Structs.h"

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

