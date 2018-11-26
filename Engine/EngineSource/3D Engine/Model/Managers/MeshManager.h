#pragma once
#pragma warning (disable : 4172)
#include <mutex>


const uint8_t MESH_HASHTABLE_SIZE = 13;

class StaticMesh;
class SkinnedMesh;

namespace ImporterLibrary
{
	struct CollisionBoxes;
};

class MeshManager
{
private:



	std::mutex m_mutexStatic;

	std::vector<StaticMesh*> m_staticMesh[MESH_HASHTABLE_SIZE];
	std::vector<SkinnedMesh*> m_skinnedMesh[MESH_HASHTABLE_SIZE];
	
public:
	MeshManager();
	~MeshManager();
	bool loadSkinnedMesh(const std::string & meshName);
	bool loadStaticMesh(const std::string & meshName);
	SkinnedMesh* getSkinnedMesh(const std::string & meshName);
	StaticMesh* getStaticMesh(const std::string & meshName);
	const ImporterLibrary::CollisionBoxes & getCollisionBoxes(const std::string & meshName);

	bool UnloadStaticMesh(const std::string & meshName);
	bool UnloadSkinnedMesh(const std::string & meshName);
	void UnloadAllMeshes();
	const unsigned int getAllLoadedMeshes() const;
private:

	unsigned int _getKey(const std::string & meshName);
	std::string _getFullPath(const std::string & meshName);
	std::string _getFullPathCollision(const std::string & meshName);
};

