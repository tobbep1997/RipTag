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
	for (int i = 0; i < m_staticMesh.size(); i++)
	{
		m_staticMesh[i]->Draw();
		if (i == 1)
		{
			m_staticMesh[i]->QueueVisabilityDraw();
		}
	}
	for (int i = 0; i < m_dynamicMesh.size(); i++)
	{
		m_dynamicMesh[i]->DrawAnimated();
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





