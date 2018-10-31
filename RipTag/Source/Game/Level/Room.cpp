#include "Room.h"
#include <AudioEngine.h>
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
		AudioEngine::ReleaseGeometry();
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
			this->m_roomGuards.push_back(new Enemy(tempGuards.startingPositions[i].startingPos[0], tempGuards.startingPositions[i].startingPos[1], tempGuards.startingPositions[i].startingPos[2]));
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
		auto boxes = Manager::g_meshManager.getCollisionBoxes(this->getAssetFilePath());
		for (unsigned int i = 0; i < boxes.nrOfBoxes; i++)
		{
			FMOD::Geometry * ge = *AudioEngine::CreateCube(1, 1);
			FMOD_RESULT res;
			float * ss = boxes.boxes[i].scale;
			FMOD_VECTOR scale = { ss[0] * 0.5f, ss[1] * 0.5f, ss[2] * 0.5f };

			// IS THIS CORRECT????
			float * r = boxes.boxes[i].rotation;
			DirectX::XMFLOAT4 q = { r[0], r[1], r[2], r[3] };

			DirectX::XMMATRIX lasoda = DirectX::XMMatrixRotationRollPitchYaw(1, 0, 0);


			// ONLY TEMP!!!
			DirectX::XMMATRIX * rotmatrix = new DirectX::XMMATRIX(DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q)));

			DirectX::XMFLOAT4X4 maa;
			DirectX::XMStoreFloat4x4(&maa, *rotmatrix);
			//DirectX::XMStoreFloat4x4(&maa, lol);

			DirectX::XMVECTOR vUp = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR vFo = DirectX::XMVectorSet(0, 0, 1, 0);
			vUp = DirectX::XMVector3Transform(vUp, *rotmatrix);
			vFo = DirectX::XMVector3Transform(vFo, *rotmatrix);

			DirectX::XMFLOAT3 xmfo, xmup;
			DirectX::XMStoreFloat3(&xmfo, vFo);
			DirectX::XMStoreFloat3(&xmup, vUp);

			ge->setUserData((void*)rotmatrix);

			FMOD_VECTOR up = { xmup.x, xmup.y, xmup.z };
			FMOD_VECTOR forward = {xmfo.x, xmfo.y, xmfo.z};
			
			float * tr = boxes.boxes[i].translation;

			FMOD_VECTOR translation = { tr[0], tr[1], tr[2] };
			
			res = ge->setScale(&scale);
			std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
			res = ge->setRotation(&forward, &up);
			//std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
			res = ge->setPosition(&translation);
			std::cout << "AudioEngine: " + std::to_string(res) + "\nMessage: " + FMOD_ErrorString(res) + "\n";
			// This is for FMOD -- END
		}

		CollisionBoxes->Init(*m_worldPtr, boxes);
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
	for (size_t i = 0; i < m_roomGuards.size(); i++)
	{
		this->m_roomGuards.at(i)->Update(0.001f);
		this->m_roomGuards.at(i)->CullingForVisability(*m_playerInRoomPtr->getTransform());
		this->m_roomGuards.at(i)->QueueForVisibility();
		this->m_roomGuards.at(i)->_IsInSight();
		vis.push_back(this->m_roomGuards.at(i)->getPlayerVisibility());
	}
	int endvis = 0;
	for (int i = 0; i < vis.size(); ++i)
	{
		endvis += vis.at(i)[0];
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
