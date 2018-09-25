#pragma once
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/Shader/ShaderManager.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/3D Engine/Model/AnimatedModel.h"


class MeshManager
{
private:

	std::vector<StaticMesh*> m_StaticMesh;
	std::vector<DynamicMesh*> m_DynamicMesh;
public:
	MeshManager();
	~MeshManager();
	bool loadDynamicMesh(const std::string & meshName);
	bool loadStaticMesh(const std::string & meshName);
	DynamicMesh* getDynamicMesh(int which /*Hashtable?*/);
	StaticMesh* getStaticMesh(int which /*Hashtable?*/);


};

