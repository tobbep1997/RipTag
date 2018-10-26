#include "Room.h"

Room::Room(const short unsigned int roomIndex, b3World * worldPtr)
{
	this->m_roomIndex = roomIndex;
	this->m_worldPtr = worldPtr;
}
Room::Room(const short unsigned int roomIndex, b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	std::string filePath = "RUM";
	filePath += std::to_string(roomIndex);
	this->m_arrayIndex = arrayIndex;
	this->m_roomIndex = roomIndex;
	this->m_playerInRoomPtr = playerPtr;

	this->m_worldPtr = worldPtr;
	setAssetFilePath(filePath);
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

DirectX::XMFLOAT4 Room::getPlayer1StartPos()
{
	return this->m_player1StartPos;
}
DirectX::XMFLOAT4 Room::getPlayer2StartPos()
{
	return this->m_player2StartPos;
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
		for (int i = 0; i < m_pointLights.size(); i++)
			delete m_pointLights[i];
		m_staticAssets.clear();
		m_pointLights.clear();
		
		m_roomLoaded = false;
	}
}

void Room::LoadRoomToMemory()
{
	//TODO:: add all the assets to whatever
	
	if (m_roomLoaded == false)
	{
		MyLibrary::Loadera fileLoader;
		MyLibrary::PointLights tempLights = fileLoader.readLightFile(this->getAssetFilePath());
		for (int i = 0; i < tempLights.nrOf; i++)
		{
			this->m_pointLights.push_back(new PointLight(tempLights.lights[i].translate, tempLights.lights[i].color, tempLights.lights[i].intensity));
		}
		delete tempLights.lights;

		MyLibrary::startingPos player1Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 1);
		MyLibrary::startingPos player2Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 2);

		m_player1StartPos = DirectX::XMFLOAT4(player1Start.startingPos[0], player1Start.startingPos[1], player1Start.startingPos[2], 1.0f);
		m_player2StartPos = DirectX::XMFLOAT4(player2Start.startingPos[0], player2Start.startingPos[1], player2Start.startingPos[2], 1.0f);

		MyLibrary::GuardStartingPositions tempGuards = fileLoader.readGuardStartFiles(this->getAssetFilePath());
		
		for (int i = 0; i < tempGuards.nrOf; i++)
		{
			this->m_roomGuards.push_back(new Enemy(m_worldPtr, tempGuards.startingPositions[i].startingPos[0], tempGuards.startingPositions[i].startingPos[1], tempGuards.startingPositions[i].startingPos[2]));
		}
		delete tempGuards.startingPositions;
		



		StaticAsset * temp = new StaticAsset();
		temp->Init(*m_worldPtr, 0.01f,0.01f,0.01f);
		//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
		temp->setPosition(0, 0, 0);
		Manager::g_meshManager.loadStaticMesh(this->getAssetFilePath());
		temp->setTexture(Manager::g_textureManager.getTexture(this->getAssetFilePath()));
		temp->setModel(Manager::g_meshManager.getStaticMesh(this->getAssetFilePath()));
		
	
		m_staticAssets.push_back(temp);
		 
		CollisionBoxes = new BaseActor();
		CollisionBoxes->Init(*m_worldPtr, Manager::g_meshManager.getCollisionBoxes(this->getAssetFilePath()));



		m_roomLoaded = true;
	
		//std::cout << "Room " << m_roomIndex << " Loaded" << std::endl;

	}
	else
	{
		//std::cout << "Room " << m_roomIndex << " Already Loaded" << std::endl;
	}
}


void Room::Update(float deltaTime)
{
	m_playerInRoomPtr->possessGuard(10);
	for (size_t i = 0; i < m_roomGuards.size(); i++)
	{
		this->m_roomGuards.at(i)->Update(0.001f);
		this->m_roomGuards.at(i)->CullingForVisability(*m_playerInRoomPtr->getTransform());
		this->m_roomGuards.at(i)->QueueForVisibility();
		this->m_roomGuards.at(i)->PhysicsUpdate(deltaTime);
		vis.push_back(this->m_roomGuards.at(i)->getPlayerVisibility());
		if ((int)this->m_roomGuards.at(i)->getBody()->GetUserData() == 3)
		{
			this->m_roomGuards.at(i)->getBody()->SetUserData((void*)2);
			this->m_roomGuards.at(i)->UnlockEnemyInput();
			this->m_playerInRoomPtr->LockPlayerInput();
		}
	}
	int endvis = 0;
	for (int i = 0; i < vis.size(); ++i)
	{
		endvis += vis.at(i)[0] / 100;
	}
	m_playerInRoomPtr->SetCurrentVisability(endvis);
	
	vis.clear();
}

void Room::Draw()
{
	for (int i = 0; i < m_staticAssets.size(); ++i)
	{
		m_staticAssets.at(i)->Draw();
	}
	for (auto light : m_pointLights)
	{
		light->QueueLight();
	}
	for (size_t i = 0; i < m_roomGuards.size(); i++)
		this->m_roomGuards.at(i)->Draw();
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
	if (CollisionBoxes)
	{
		CollisionBoxes->Release(*m_worldPtr);
		delete CollisionBoxes;
		CollisionBoxes = nullptr;
	}
	for (auto light : m_pointLights)
	{
		delete light;
	}
	for (auto enemy : m_roomGuards)
	{
		delete enemy;
	}
	
	
}

void Room::loadTextures()
{
	Manager::g_textureManager.loadTextures(this->getAssetFilePath());
}
