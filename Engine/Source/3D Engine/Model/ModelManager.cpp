#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	/*for (int i = 0; i < dynamicMesh.size(); i++)
	{
		delete dynamicMesh[i];
	}
	for (int i = 0; i < staticMesh.size(); i++)
	{
		delete staticMesh[i];
	}*/
	for (int i = 0; i < staticModel.size(); i++)
	{
		delete staticModel[i];
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
		dynamicModel[i].DrawAnimated();
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







