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
		tempMesh->LoadCollision(this->_getFullPathCollision(meshName));
		m_mutexStatic.lock();
		m_staticMesh[key].push_back(tempMesh);
		m_mutexStatic.unlock();
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
			tempMesh->LoadCollision(this->_getFullPathCollision(meshName));
			m_mutexStatic.lock();
			m_staticMesh[key].push_back(tempMesh);
			m_mutexStatic.unlock();
		}
		else
		{
			//Mesh already loaded
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

const MyLibrary::CollisionBoxes & MeshManager::getCollisionBoxes(const std::string & meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_staticMesh[key].size(); i++)
	{
		if (m_staticMesh[key][i]->getName() == fullPath)
		{
			return m_staticMesh[key][i]->getCollisionBoxes();
		}
	}
	return MyLibrary::CollisionBoxes();
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

bool MeshManager::UnloadStaticMesh(const std::string& meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_staticMesh[key].size(); i++)
	{
		if (m_staticMesh[key].at(i)->getName() == fullPath)
		{
			delete m_staticMesh[key].at(i);
			m_staticMesh[key].erase(m_staticMesh[key].begin() + i);
			return true;
		}
	}
	return false;
}

bool MeshManager::UnloadDynamicMesh(const std::string& meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_dynamicMesh[key].size(); i++)
	{
		if (m_dynamicMesh[key].at(i)->getName() == fullPath)
		{
			delete m_dynamicMesh[key].at(i);
			m_dynamicMesh[key].erase(m_dynamicMesh[key].begin() + i);
			return true;
		}
	}
	return false;
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

std::string MeshManager::_getFullPathCollision(const std::string & meshName)
{
	std::string tempString = "../Assets/";
	tempString.append(meshName + "FOLDER/" + meshName + "_BBOX.bin");
	return tempString;
}
