#include "EnginePCH.h"
#include "ModelManager.h"


#include "ImportLibrary/formatImporter.h"


ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	for (int i = 0; i < m_dynamicModel.size(); i++)
	{
		delete m_dynamicModel[i];
	}
	for (int i = 0; i < m_staticModel.size(); i++)
	{
		delete m_staticModel[i];
	}
}

void ModelManager::DrawMeshes()
{
	for (int i = 0; i < m_staticModel.size(); i++)
	{
		m_staticModel[i]->Draw();
	}
	for (int i = 0; i < m_dynamicModel.size(); i++)
	{
		m_dynamicModel[i]->Draw();
	}
}

void ModelManager::addNewModel(StaticMesh* mesh, Texture* texture)
{
	Drawable *tempModel = new Drawable();
	tempModel->setModel(mesh);
	tempModel->setTexture(texture);
	m_staticModel.push_back(tempModel);
}
void ModelManager::addNewModel(DynamicMesh* mesh, Texture* texture)
{
	Drawable *tempModel = new Drawable();
	tempModel->setModel(mesh);
	tempModel->setTexture(texture);
	m_dynamicModel.push_back(tempModel);
}







