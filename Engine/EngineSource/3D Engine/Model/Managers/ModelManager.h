#pragma once
//#include "EngineSource/3D Engine/RenderingManager.h"
//#include "EngineSource/Shader/ShaderManager.h"
//#include "EngineSource/3D Engine/Model/Model.h"
//#include "EngineSource/3D Engine/RenderingManager.h"
//#include "EngineSource/3D Engine/Model/Meshes/AnimatedModel.h"
#include "../../Components/Base/Drawable.h"

class ModelManager
{
private:
	

public:
	ModelManager();
	~ModelManager();
	
	std::vector<Drawable*> m_staticModel;
	std::vector<Drawable*> m_dynamicModel;
	
	void DrawMeshes();
	void addNewModel(StaticMesh* mesh, Texture* texture);
	void addNewModel(DynamicMesh* mesh, Texture* texture);


};
