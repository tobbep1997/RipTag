#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	for (int i = 0; i < dynamicMesh.size(); i++)
	{
		delete dynamicMesh[i];
	}
	for (int i = 0; i < staticMesh.size(); i++)
	{
		delete staticMesh[i];
	}
}

void ModelManager::DrawMeshes()
{
	for (int i = 0; i < staticMesh.size(); i++)
	{
		staticMesh[i]->Draw();
		if (i == 1)
		{
			staticMesh[i]->QueueVisabilityDraw();
		}
	}
	for (int i = 0; i < dynamicMesh.size(); i++)
	{
		dynamicMesh[i]->DrawAnimated();
	}
}

void ModelManager::addStaticMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Static, 
		assetFilePath
		);
	
	staticMesh.push_back(tempModel);
}

void ModelManager::addDynamicMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Dynamic,
		assetFilePath
		);

	dynamicMesh.push_back(tempModel);
}





