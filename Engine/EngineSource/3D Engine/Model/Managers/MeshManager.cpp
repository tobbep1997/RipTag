#include "EnginePCH.h"
#include "MeshManager.h"



MeshManager::MeshManager()
{
}


MeshManager::~MeshManager()
{
	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_skinnedMesh[i].size(); j++)
		{
			delete m_skinnedMesh[i][j];
		}
		m_skinnedMesh[i].clear();
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

bool MeshManager::loadSkinnedMesh(const std::string & meshName)
{
	SkinnedMesh* tempMesh = new SkinnedMesh();
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);
	if (m_staticMesh[key].size() == 0)
	{
		tempMesh->setName(fullPath);
		tempMesh->LoadMesh(fullPath);
		m_skinnedMesh[key].push_back(tempMesh);
	}
	else
	{
		bool duplicate = false;
		for (unsigned int i = 0; i < m_skinnedMesh[key].size(); i++)
		{
			if (m_skinnedMesh[key].at(i)->getName() == fullPath)
			{
				duplicate = true;
			}
		}
		if (duplicate == false)
		{
			tempMesh->setName(fullPath);
			tempMesh->LoadMesh(fullPath);
			m_skinnedMesh[key].push_back(tempMesh);
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

bool MeshManager::loadStaticMesh(const std::string & meshName)
{
	StaticMesh* tempMesh = new StaticMesh();
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	if (m_staticMesh[key].size() == 0)
	{		
		tempMesh->setName(fullPath);
		tempMesh->LoadMesh(fullPath);
		//tempMesh->LoadCollision(this->_getFullPath(meshName));
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
			//tempMesh->LoadCollision(this->_getFullPathCollision(meshName));
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

SkinnedMesh * MeshManager::getSkinnedMesh(const std::string & meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_skinnedMesh[key].size(); i++)
	{
		if (m_skinnedMesh[key][i]->getName() == fullPath)
		{
			return m_skinnedMesh[key][i];
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

const ImporterLibrary::CollisionBoxes & MeshManager::getCollisionBoxes(const std::string & meshName)
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
	return ImporterLibrary::CollisionBoxes();
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

bool MeshManager::UnloadSkinnedMesh(const std::string& meshName)
{
	std::string fullPath = this->_getFullPath(meshName);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_skinnedMesh[key].size(); i++)
	{
		if (m_skinnedMesh[key].at(i)->getName() == fullPath)
		{
			delete m_skinnedMesh[key].at(i);
			m_skinnedMesh[key].erase(m_skinnedMesh[key].begin() + i);
			return true;
		}
	}
	return false;
}

void MeshManager::UnloadAllMeshes()
{
	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_skinnedMesh[i].size(); j++)
		{
			delete m_skinnedMesh[i][j];
		}
		m_skinnedMesh[i].clear();
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

const unsigned int MeshManager::getAllLoadedMeshes() const
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)	
		for (unsigned int j = 0; j < m_skinnedMesh[i].size(); j++)		
			count++;

	for (unsigned int i = 0; i < MESH_HASHTABLE_SIZE; i++)	
		for (unsigned int j = 0; j < m_staticMesh[i].size(); j++)		
			count++;
	return count;
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
