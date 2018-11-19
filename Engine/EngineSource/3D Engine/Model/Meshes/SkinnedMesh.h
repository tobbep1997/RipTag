#pragma once
#include <vector>

struct DynamicVertex;

class SkinnedMesh 
{
private:
	std::vector <DynamicVertex> m_dynamicVertex;
	std::string m_meshName;
public:
	SkinnedMesh();
	~SkinnedMesh();
	const DynamicVertex * getRawVertices() const;
	const std::vector<DynamicVertex> & getVertices() const;
	void setVertices(std::vector<DynamicVertex>& input);

	void SET_DEFAULT();

	void setName(const std::string & name);
	const std::string & getName() const;

	void LoadMesh(const std::string & path);
};

