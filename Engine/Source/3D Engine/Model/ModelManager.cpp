#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
}

void ModelManager::DrawMeshes()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Draw();
	}
}

void ModelManager::addStaticMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Static, 
		assetFilePath
		);
	
	
	tempModel->setScale(0.1, 0.1, 0.1);
	
	meshes.push_back(tempModel);
}

void ModelManager::addDynamicMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Dynamic,
		assetFilePath
		);
	

	
	tempModel->setScale(1, 1, 1);

	meshes.push_back(tempModel);
}





