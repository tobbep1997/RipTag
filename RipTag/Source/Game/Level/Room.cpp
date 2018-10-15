#include "Room.h"

Room::Room(const short unsigned int roomIndex, b3World * worldPtr)
{
	this->m_roomIndex = roomIndex;
	this->m_worldPtr = worldPtr;
}

Room::~Room()
{
	
}

void Room::setRoomIndex(const short unsigned int roomIndex)
{
	this->m_roomIndex = roomIndex;
}

unsigned short int Room::getRoomIndex()
{
	return this->m_roomIndex;
}

void Room::setAssetFilePath(const std::string& fileName)
{
	m_assetFilePath = fileName;
}

std::string Room::getAssetFilePath()
{
	return m_assetFilePath;
}

const bool Room::getRoomLoaded()
{
	return m_roomLoaded;
}

void Room::UnloadRoomFromMemory()
{
	if (m_roomLoaded == true)
	{
		for (auto asset : m_staticAssets)
		{
			asset->Release(*m_worldPtr);
		}
		for (auto asset : m_staticAssets)
		{
			delete asset;
		}
		m_staticAssets.clear();
		m_roomLoaded = false;

		//std::cout << "Room " << m_roomIndex << " UNLoaded" << std::endl;
	}
}

void Room::LoadRoomToMemory()
{
	//TODO:: add all the assets to whatever
	
	if (m_roomLoaded == false)
	{
	
		if (m_roomIndex == 0)
		{
		StaticAsset * temp = new StaticAsset();
		temp->Init(*m_worldPtr,1,1,1);
		//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
		temp->setPosition(5, 5.0f, 0);
		temp->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
		temp->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	
		m_staticAssets.push_back(temp);
	
		}
		else
		{
			StaticAsset * temp = new StaticAsset();
			temp->Init(*m_worldPtr, 1, 1, 1);
			//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
			temp->setPosition(10, 5.0f, 0);
			temp->setModel(Manager::g_meshManager.getStaticMesh("KUB"));
			temp->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	
			m_staticAssets.push_back(temp);
		}
		m_roomLoaded = true;
	
		//std::cout << "Room " << m_roomIndex << " Loaded" << std::endl;

	}
	else
	{
		//std::cout << "Room " << m_roomIndex << " Already Loaded" << std::endl;
	}
}


void Room::Update()
{
}

void Room::Draw()
{
	for (int i = 0; i < m_staticAssets.size(); ++i)
	{
		m_staticAssets.at(i)->Draw();
	}
}

void Room::Release()
{
	for (auto asset : m_staticAssets)
	{
		asset->Release(*m_worldPtr);
	}
	for (auto asset : m_staticAssets)
	{
		delete asset;
	}
}
