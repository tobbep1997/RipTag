#pragma once
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/Shader/ShaderManager.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/3D Engine/Model/AnimatedModel.h"


class ModelManager
{
private:
	

public:
	ModelManager();
	~ModelManager();
	
	
	std::vector<Model*> meshes;
	
	void DrawMeshes();
	void addStaticMesh(const std::string & assetFilePath);
	void addDynamicMesh(const std::string& assetFilePath);
	

};

