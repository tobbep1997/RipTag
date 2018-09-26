#include "ModelManager.h"



ModelManager::ModelManager()
{
	

}


ModelManager::~ModelManager()
{
	for (int i = 0; i < m_dynamicMesh.size(); i++)
	{
		delete m_dynamicMesh[i];
	}
	for (int i = 0; i < m_staticMesh.size(); i++)
	{
		delete m_staticMesh[i];
	}
}

void ModelManager::DrawMeshes()
{
	//TODO::FIX GUARD PASS
	for (int i = 0; i < m_staticMesh.size(); i++)
	{
		switch (m_staticMesh[i]->getEntityType())
		{
		case EntityType::Defult:
			m_staticMesh[i]->Draw();
			break;
		case EntityType::Player:
			m_staticMesh[i]->QueueVisabilityDraw();
			m_staticMesh[i]->Draw();
			break;
		case EntityType::Guardd:

			m_staticMesh[i]->Draw();
			break;
		}
		
		
	}
	for (int i = 0; i < m_dynamicMesh.size(); i++)
	{
		switch (m_dynamicMesh[i]->getEntityType())
		{
		case EntityType::Defult:
			m_dynamicMesh[i]->DrawAnimated();
			break;
		case EntityType::Player:

			break;
		case EntityType::Guardd:

			break;
		}
		//m_dynamicMesh[i]->DrawAnimated();
	}
}

void ModelManager::addStaticMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Static, 
		assetFilePath
		);
	
	m_staticMesh.push_back(tempModel);
}

void ModelManager::addDynamicMesh(const std::string & assetFilePath)
{
	Model * tempModel = new Model(ObjectType::Dynamic,
		assetFilePath
		);

	m_dynamicMesh.push_back(tempModel);
}





