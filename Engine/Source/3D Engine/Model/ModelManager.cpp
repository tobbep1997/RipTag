#include "ModelManager.h"



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
		if (i == 1)
		{
			m_staticModel[i]->QueueVisabilityDraw();
		}
	}
	for (int i = 0; i < m_dynamicModel.size(); i++)
	{
		m_dynamicModel[i]->Draw();
	}
}

void ModelManager::addNewModel(StaticMesh* mesh, Texture* texture)
{
	Model *tempModel = new Model();
	tempModel->setModel(mesh);
	tempModel->setTexture(texture);
	m_staticModel.push_back(tempModel);
}
void ModelManager::addNewModel(DynamicMesh* mesh, Texture* texture)
{
	Model *tempModel = new Model(Dynamic);
	tempModel->setModel(mesh);
	tempModel->setTexture(texture);
	m_dynamicModel.push_back(tempModel);
}







