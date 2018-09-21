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
	
	std::vector<Model> dynamicModel;
	std::vector<Model*> staticModel;

	void DrawMeshes();
	void bindTextures(const std::string& assetName);
	void addNewModel(StaticMesh* mesh, Texture* texture);
	

};

