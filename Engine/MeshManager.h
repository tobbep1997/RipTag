#pragma once
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/Shader/ShaderManager.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/3D Engine/Model/AnimatedModel.h"


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

private:

	unsigned int _getKey(const std::string & meshName);
	std::string _getFullPath(const std::string & meshName);
};

