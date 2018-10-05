#pragma once
#include "../Meshes/StaticMesh.h"
#include "../Meshes/DynamicMesh.h"

const uint8_t MESH_HASHTABLE_SIZE = 13;
class MeshManager
{
private:

	std::vector<StaticMesh*> m_staticMesh[MESH_HASHTABLE_SIZE];
	std::vector<DynamicMesh*> m_dynamicMesh[MESH_HASHTABLE_SIZE];
	
public:
	MeshManager();
	~MeshManager();
	bool loadDynamicMesh(const std::string & meshName);
	bool loadStaticMesh(const std::string & meshName);
	DynamicMesh* getDynamicMesh(const std::string & meshName);
	StaticMesh* getStaticMesh(const std::string & meshName);
	void UpdateAllAnimations(float deltaTime);

private:

	unsigned int _getKey(const std::string & meshName);
	std::string _getFullPath(const std::string & meshName);
};

