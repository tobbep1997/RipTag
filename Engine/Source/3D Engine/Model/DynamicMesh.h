#pragma once
#include <vector>
#include "../../Structs.h"
#pragma comment(lib, "New_Library.lib")
#include <../New_Library/formatImporter.h>
class DynamicMesh
{
private:
	//std::vector<StaticVertex> staticVertex;
	std::vector <DynamicVertex> dynamicVertex;
	//TODO 
public:
	DynamicMesh();
	~DynamicMesh();
	const DynamicVertex * getRawVertices() const;
	const std::vector<DynamicVertex> & getVertices() const;

	void setVertices(std::vector<DynamicVertex>& input);

	void SET_DEFAULT();

	void LoadModel(const std::string & path);
	/*
	const StaticVertex * getRawVertice() const;
	const std::vector<StaticVertex> & getVertice() const;

	void setVertices(std::vector<StaticVertex>& input);
	*/
};

