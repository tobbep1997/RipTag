#include "Room.h"
#include "EngineSource/3D Engine/RenderingManager.h"

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
	triggerHandler = new TriggerHandler();

	Door * door;
	Lever * lever;
	PressurePlate * pressurePlate;

	door = new Door();
	door->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	door->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	door->Init(*m_worldPtr, e_staticBody);
	door->setPos(DirectX::XMFLOAT4A(-25.46f, 4.306f, -7.692f,1), DirectX::XMFLOAT4A(-9.46f, 4.306f, -7.692f, 1));
	pressurePlate = new PressurePlate();
	Manager::g_meshManager.loadStaticMesh("PRESSUREPLATE");
	Manager::g_textureManager.loadTextures("PRESSUREPLATE");
	pressurePlate->setScale(2, 1, 2);
	pressurePlate->setModel(Manager::g_meshManager.getStaticMesh("PRESSUREPLATE"));
	pressurePlate->setTexture(Manager::g_textureManager.getTexture("PRESSUREPLATE"));
	pressurePlate->Init();
	pressurePlate->setPos(DirectX::XMFLOAT4A(7.327f, 4.294f, 13.764f, 1), DirectX::XMFLOAT4A(7.327f, 4.2f, 13.764f, 1));
	
	lever = new Lever();
	lever->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	lever->setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	lever->Init();
	lever->setPosition(5, 10, 5);
	
	std::vector<Trigger*> t0;
	std::vector<Triggerble*> t1;
	t0.push_back(pressurePlate);
	t0.push_back(lever);
	t1.push_back(door);

	triggerHandler->AddPair(t0, t1, true);
	baseActors.push_back(door);
	baseActors.push_back(pressurePlate);
	baseActors.push_back(lever);
	triggers.push_back(pressurePlate);
	levers.push_back(lever);

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

		CollisionBoxes->Release(*RipExtern::g_world);
		delete CollisionBoxes;

		for (auto & ab : m_audioBoxes)
			ab->release();
		m_audioBoxes.clear();
		delete m_grid.gridPoints;
		m_roomLoaded = false;


	}

}

void Room::LoadRoomToMemory()
{
	//TODO:: add all the assets to whatever
	std::cout << m_assetFilePath << std::endl;
	if (m_roomLoaded == false)
	{
		MyLibrary::Loadera fileLoader;
		MyLibrary::PointLights tempLights = fileLoader.readLightFile(this->getAssetFilePath());
		for (int i = 0; i < tempLights.nrOf; i++)
		{
			this->m_pointLights.push_back(new PointLight(tempLights.lights[i].translate, tempLights.lights[i].color, tempLights.lights[i].intensity));
		}
		delete tempLights.lights;


		m_grid = fileLoader.readGridFile(this->getAssetFilePath());
		m_pathfindingGrid.CreateGridWithWorldPosValues(m_grid.maxX, m_grid.maxY, m_grid);
		//
		m_roomLoaded = true;

		MyLibrary::StartingPos player1Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 1);
		MyLibrary::StartingPos player2Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 2);

		m_player1StartPos = DirectX::XMFLOAT4(player1Start.startingPos[0], player1Start.startingPos[1], player1Start.startingPos[2], 1.0f);
		m_player2StartPos = DirectX::XMFLOAT4(player2Start.startingPos[0], player2Start.startingPos[1], player2Start.startingPos[2], 1.0f);

		MyLibrary::GuardStartingPositions tempGuards = fileLoader.readGuardStartFiles(this->getAssetFilePath());

		for (int i = 0; i < tempGuards.nrOf; i++)
		{
			this->m_roomGuards.push_back(new Enemy(m_worldPtr, tempGuards.startingPositions[i].startingPos[0], tempGuards.startingPositions[i].startingPos[1], tempGuards.startingPositions[i].startingPos[2]));

			/*std::vector<Node*> path = m_pathfindingGrid.FindPath(Tile(0, 0), Tile(24, 13));
			this->m_roomGuards.at(i)->SetPathVector(path);*/
			//this->m_roomGuards.at(i)->setPosition(-10, 0, -10);
		}
		delete tempGuards.startingPositions;

		//getPath();


		StaticAsset * temp = new StaticAsset();
		temp->Init(*m_worldPtr, 1, 1, 1);
		//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
		temp->setPosition(0, 0, 0);
		Manager::g_meshManager.loadStaticMesh(this->getAssetFilePath());
		temp->setTexture(Manager::g_textureManager.getTexture(this->getAssetFilePath()));
		temp->setModel(Manager::g_meshManager.getStaticMesh(this->getAssetFilePath()));

		CollisionBoxes = new BaseActor();
		auto boxes = Manager::g_meshManager.getCollisionBoxes(this->getAssetFilePath());
		CollisionBoxes->Init(*m_worldPtr, boxes);
		CollisionBoxes->addCollisionBox(b3Vec3(0.066f, 5.35f, -0.644f), b3Vec3(0.798f, 3.052f, 3.052f), b3Quaternion(0, 0, 0, 0), "BLINK_WALL", false, m_worldPtr);
		CollisionBoxes->addCollisionBox(b3Vec3(-6.149f, 6.17f, -9.57f), b3Vec3(10.1323f, 4.385f, 1.052f), b3Quaternion(0,0,0,0), "BLINK_WALL", false, m_worldPtr);

		for (unsigned int i = 0; i < boxes.nrOfBoxes; i++)
		{
			float * f4Rot = boxes.boxes[i].rotation;
			float * f3Pos = boxes.boxes[i].translation;
			float * f3Scl = boxes.boxes[i].scale;
			DirectX::XMFLOAT4 xmQ = { f4Rot[0], f4Rot[1], f4Rot[2], f4Rot[3] };
			DirectX::XMFLOAT4 xmPos = { f3Pos[0], f3Pos[1], f3Pos[2], 1};
			DirectX::XMFLOAT4 xmScl = { f3Scl[0] * 0.5f, f3Scl[1] * 0.5f, f3Scl[2] * 0.5f, 1};
			FMOD::Geometry * ge = AudioEngine::CreateCube(0.75f, 0.35f, xmPos, xmScl, xmQ);
			ge->setActive(false);
			m_audioBoxes.push_back(ge);
		}

		m_staticAssets.push_back(temp);
		 

	
		
		m_roomLoaded = true;
	
		//std::cout << "Room " << m_roomIndex << " Loaded" << std::endl;
	}
	else
	{
		//std::cout << "Room " << m_roomIndex << " Already Loaded" << std::endl;
	}

	for (auto light : m_pointLights)
	{
		light->setColor(200, 102, 50);
	}
}

void Room::getPath()
{
	std::vector<Node*> path = m_pathfindingGrid.FindPath(Tile(0, 0), Tile(24, 13));
	std::cout << "Printing path..." << std::endl << std::endl;
	for (int i = 0; i < path.size(); i++)
	{
		std::cout << "x: " << path.at(i)->tile.getX() << " y: " << path.at(i)->tile.getY() << std::endl;
		std::cout << "World x: " << path.at(i)->worldPos.x << " World y: " << path.at(i)->worldPos.y << std::endl;
	}
	std::cout << std::endl << "Path is finished printing..." << std::endl;
	for (int i = 0; i < path.size(); i++)
	{
		delete path.at(i);
		path.at(i) = nullptr;
	}
}

void Room::Update(float deltaTime)
{
	for (size_t i = 0; i < m_roomGuards.size(); i++)
	{
		this->m_roomGuards.at(i)->Update(deltaTime);
		this->m_roomGuards.at(i)->CullingForVisability(*m_playerInRoomPtr->getTransform());
		this->m_roomGuards.at(i)->QueueForVisibility();
		this->m_roomGuards.at(i)->_IsInSight();
		this->m_roomGuards.at(i)->PhysicsUpdate(deltaTime);
		vis.push_back(this->m_roomGuards.at(i)->getPlayerVisibility());

	}
	int endvis = 0;
	
	for (int i = 0; i < vis.size(); ++i)
	{
		
		if (vis.at(i)[0] >= 1)
		{
			endvis += vis.at(i)[0];
		}
	}
	m_playerInRoomPtr->SetCurrentVisability(endvis);
	
	vis.clear();

	for (auto light : m_pointLights)
	{
		
		light->setIntensity(light->TourchEffect(deltaTime * .1f, 0.1f, 1));
	}
	triggerHandler->Update(deltaTime);
	for (int i = 0; i < triggers.size(); i++)
	{
		triggers[i]->Update(deltaTime);
	}
	for (int i = 0; i < levers.size(); i++)
	{
		levers[i]->Update(deltaTime);
	}
}

void Room::SetActive(bool state)
{
	for (auto & ab : m_audioBoxes)
		ab->setActive(state);
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
	for (int i = 0; i < baseActors.size(); i++)
	{
		baseActors.at(i)->Draw();
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
	for (auto ab : m_audioBoxes)
		ab->release();
	delete m_grid.gridPoints;

	for (int i = 0; i < baseActors.size(); i++)
	{
		baseActors[i]->Release(*m_worldPtr);
		delete baseActors[i];
	}
	delete triggerHandler;
	
}

void Room::loadTextures()
{
	Manager::g_textureManager.loadTextures(this->getAssetFilePath());
}
