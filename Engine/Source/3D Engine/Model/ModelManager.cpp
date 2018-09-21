#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	
	for (int i = 0; i < staticModel.size(); i++)
	{
		delete staticModel[i];
	}
	for (int i = 0; i < dynamicModel.size(); i++)
	{
		delete dynamicModel[i];
	}
}

void ModelManager::DrawMeshes()
{
	for (int i = 0; i < staticModel.size(); i++)
	{
		staticModel[i]->Draw();
	}
	for (int i = 0; i < dynamicModel.size(); i++)
	{
		dynamicModel[i]->DrawAnimated();
	}
}

void ModelManager::bindTextures(const std::string& assetName)
{

}

void ModelManager::addNewModel(StaticMesh* mesh, Texture* texture)
{
	Model *tempModel = new Model();
	tempModel->SetModel(mesh);
	tempModel->setTexture(texture);
	staticModel.push_back(tempModel);
}
void ModelManager::addNewModel(DynamicMesh* mesh, Texture* texture)
{
	Model *tempModel = new Model(Dynamic);
	tempModel->SetModel(mesh);
	tempModel->setTexture(texture);
	dynamicModel.push_back(tempModel);
}







