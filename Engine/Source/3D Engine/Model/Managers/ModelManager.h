#pragma once
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/Shader/ShaderManager.h"
#include "../Engine/Source/3D Engine/Model/Model.h"
#include "../Engine/Source/3D Engine/RenderingManager.h"
#include "../Engine/Source/3D Engine/Model/Meshes/AnimatedModel.h"


class ModelManager
{
private:
	

public:
	ModelManager();
	~ModelManager();
	
	std::vector<Model*> m_staticModel;
	std::vector<Model*> m_dynamicModel;
	
	void DrawMeshes();
	void addNewModel(StaticMesh* mesh, Texture* texture);
	void addNewModel(DynamicMesh* mesh, Texture* texture);


};