#pragma once
#pragma warning (disable : 4172)
#include <mutex>


const uint8_t MESH_HASHTABLE_SIZE = 13;

class StaticMesh;
class DynamicMesh;

namespace ImporterLibrary
{
	struct CollisionBoxes;
};

class MeshManager
{
private:



	std::mutex m_mutexStatic;

	std::vector<StaticMesh*> m_staticMesh[MESH_HASHTABLE_SIZE];
	std::vector<DynamicMesh*> m_dynamicMesh[MESH_HASHTABLE_SIZE];
	
public:
	MeshManager();
	~MeshManager();
	bool loadDynamicMesh(const std::string & meshName);
	bool loadStaticMesh(const std::string & meshName);
	DynamicMesh* getDynamicMesh(const std::string & meshName);
	StaticMesh* getStaticMesh(const std::string & meshName);
	const ImporterLibrary::CollisionBoxes & getCollisionBoxes(const std::string & meshName);

	bool UnloadStaticMesh(const std::string & meshName);
	bool UnloadDynamicMesh(const std::string & meshName);
	void UnloadAllMeshes();
	const unsigned int getAllLoadedMeshes() const;
private:

	unsigned int _getKey(const std::string & meshName);
	std::string _getFullPath(const std::string & meshName);
	std::string _getFullPathCollision(const std::string & meshName);
};

