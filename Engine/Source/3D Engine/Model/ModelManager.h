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
	
	std::vector<Model*> staticMesh;
	std::vector<Model*> dynamicMesh;
	std::vector<Texture> meshtextures;
	void DrawMeshes();
	void bindTextures(const std::string& assetName);
	void addStaticMesh(const std::string & assetFilePath);
	void addDynamicMesh(const std::string& assetFilePath);
	

};

