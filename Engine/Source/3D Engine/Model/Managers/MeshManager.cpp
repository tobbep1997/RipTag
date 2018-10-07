#include "MeshManager.h"



MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_dynamicMesh[i].size(); j++)
		{
			delete m_dynamicMesh[i][j];
		}
		m_dynamicMesh[i].clear();
	}
	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_staticMesh[i].size(); j++)
		{
			delete m_staticMesh[i][j];
		}
		m_staticMesh[i].clear();
	}
}

bool MeshManager::loadDynamicMesh(const std::string & meshName)
{
	DynamicMesh* tempMesh = new DynamicMesh();
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	tempMesh->setName(fullPath);
	tempMesh->LoadMesh(fullPath);

	m_dynamicMesh[key].push_back(tempMesh);	
	return true;
}

bool MeshManager::loadStaticMesh(const std::string & meshName)
{
	StaticMesh* tempMesh = new StaticMesh();
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);
	if (m_staticMesh[key].size() == 0)
	{
		tempMesh->setName(fullPath);
		tempMesh->LoadMesh(fullPath);

		m_staticMesh[key].push_back(tempMesh);
	}
	else
	{
		bool duplicate = false;
		for (unsigned int i = 0; i < m_staticMesh[key].size(); i++)
		{
			if(m_staticMesh[key].at(i)->getName() == fullPath)
			{
				duplicate = true;
			}
		}
		if (duplicate == false)
		{
			tempMesh->setName(fullPath);
			tempMesh->LoadMesh(fullPath);

			m_staticMesh[key].push_back(tempMesh);
		}
		else
		{
			//Mesh already loaded
			std::cout << "Mesh " << fullPath << "Already loaded" << std::endl;
			delete tempMesh;
			return false;
		}
	}
	
	return true;
}

DynamicMesh * MeshManager::getDynamicMesh(const std::string & meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_dynamicMesh[key].size(); i++)
	{
		if (m_dynamicMesh[key][i]->getName() == fullPath)
		{
			return m_dynamicMesh[key][i];
		}
	}
	return nullptr;
}

StaticMesh * MeshManager::getStaticMesh(const std::string & meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_staticMesh[key].size(); i++)
	{
		if (m_staticMesh[key][i]->getName() == fullPath)
		{
			return m_staticMesh[key][i];
		}
	}
	return nullptr;
}


void MeshManager::UpdateAllAnimations(float deltaTime)
{
	//for (auto& dynamicMeshVector : m_dynamicMesh)
	//{
	//	for (auto& mesh : dynamicMeshVector)
	//		mesh->getAnimatedModel()->Update(deltaTime);
	//}

	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_dynamicMesh[i].size(); j++)
		{
			auto animatedModelPtr = m_dynamicMesh[i][j]->getAnimatedModel();
			if (animatedModelPtr)
				animatedModelPtr->Update(deltaTime);
		}
	}
}

unsigned int MeshManager::_getKey(const std::string & meshName)
{
	unsigned int sum = 0;
	for (unsigned int i = 0; i < meshName.size(); i++)
	{
		sum += meshName[i];
	}
	return sum % MESH_HASHTABLE_SIZE;
}

std::string MeshManager::_getFullPath(const std::string & meshName)
{
	std::string tempString = "../Assets/";
	tempString.append(meshName + "FOLDER/" + meshName + ".bin");
	return tempString;
}
