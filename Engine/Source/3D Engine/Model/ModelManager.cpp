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
	//TODO::FIX GUARD PASS
	for (int i = 0; i < m_staticModel.size(); i++)
	{
		switch (m_staticModel[i]->getEntityType())
		{
		case EntityType::Defult:
			m_staticModel[i]->Draw();
			break;
		case EntityType::Player:
			m_staticModel[i]->QueueVisabilityDraw();
			m_staticModel[i]->Draw();
			break;
		case EntityType::Guardd:

			m_staticModel[i]->Draw();
			break;
		}
		
		
	}
	for (int i = 0; i < m_dynamicModel.size(); i++)
	{
		switch (m_dynamicModel[i]->getEntityType())
		{
		case EntityType::Defult:
			m_dynamicModel[i]->DrawAnimated();
			break;
		case EntityType::Player:

			break;
		case EntityType::Guardd:

			break;
		}
		//m_dynamicMesh[i]->DrawAnimated();
	}
}

void ModelManager::bindTextures(const std::string& assetName)
{

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







