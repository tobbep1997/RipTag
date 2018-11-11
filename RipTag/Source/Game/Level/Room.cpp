#include "RipTagPCH.h"
#include "Room.h"

void Room::placeRoomProps(ImporterLibrary::PropItemToEngine propsToPlace)
{
	std::pair<Trigger*, Door> doorLeverPair;
	Lever * tempLever = nullptr;
	Door * tempDoor = nullptr;
	PressurePlate * tempPressurePlate = nullptr;
	Bars * tempBars = nullptr;
	
	for (int i = 0; i < propsToPlace.nrOfItems; i++)
	{
		int a = propsToPlace.props[i].typeOfProp;
		switch (propsToPlace.props[i].typeOfProp)
		{
		case(1):
			
			break;
		case(2):
			Manager::g_meshManager.loadDynamicMesh("PLATE");
			Manager::g_textureManager.loadTextures("PRESSUREPLATE");
			Manager::g_animationManager.loadSkeleton("../Assets/PLATEFOLDER/PLATE_SKELETON.bin", "PLATE");
			Manager::g_animationManager.loadClipCollection("PLATE", "PLATE", "../Assets/PLATEFOLDER", Manager::g_animationManager.getSkeleton("PLATE"));
			tempPressurePlate = new PressurePlate(i, propsToPlace.props[i].linkedItem, propsToPlace.props[i].isTrigger);

			tempPressurePlate->Init(propsToPlace.props[i].transform_position[0],
				propsToPlace.props[i].transform_position[1],
				propsToPlace.props[i].transform_position[2],
				propsToPlace.props[i].transform_rotation[0],
				propsToPlace.props[i].transform_rotation[1],
				propsToPlace.props[i].transform_rotation[2],
				propsToPlace.props[i].BBOX_INFO[0],
				propsToPlace.props[i].BBOX_INFO[1],
				propsToPlace.props[i].BBOX_INFO[2],
				propsToPlace.props[i].transform_scale[0],
				propsToPlace.props[i].transform_scale[1],
				propsToPlace.props[i].transform_scale[2]);
			triggerHandler->Triggers.push_back(tempPressurePlate);
			triggerHandler->netWorkTriggers.insert(std::pair<int, Trigger*>(i, tempPressurePlate));
			tempPressurePlate = nullptr;

			break;
		case(3):
			Manager::g_meshManager.loadDynamicMesh("DOOR");
			Manager::g_textureManager.loadTextures("DOOR");
			Manager::g_animationManager.loadSkeleton("../Assets/DOORFOLDER/DOOR_SKELETON.bin", "DOOR");
			Manager::g_animationManager.loadClipCollection("DOOR", "DOOR", "../Assets/DOORFOLDER", Manager::g_animationManager.getSkeleton("DOOR"));
			tempDoor = new Door(i, propsToPlace.props[i].linkedItem, propsToPlace.props[i].isTrigger);
			tempDoor->Init(propsToPlace.props[i].transform_position[0],
				propsToPlace.props[i].transform_position[1],
				propsToPlace.props[i].transform_position[2],
				propsToPlace.props[i].transform_rotation[0],
				propsToPlace.props[i].transform_rotation[1],
				propsToPlace.props[i].transform_rotation[2],
				propsToPlace.props[i].BBOX_INFO[0],
				propsToPlace.props[i].BBOX_INFO[1],
				propsToPlace.props[i].BBOX_INFO[2],
				propsToPlace.props[i].transform_scale[0],
				propsToPlace.props[i].transform_scale[1],
				propsToPlace.props[i].transform_scale[2]);
			triggerHandler->Triggerables.push_back(tempDoor);
			tempDoor = nullptr;
			//ladda in d�rr etc etc 
			break;
		case(4):
			//Manager::g_meshManager.loadStaticMesh("SPAK");
			//Manager::g_textureManager.loadTextures("SPAK");
			Manager::g_meshManager.loadDynamicMesh("SPAK");
			Manager::g_textureManager.loadTextures("SPAK");
			Manager::g_animationManager.loadSkeleton("../Assets/SPAKFOLDER/SPAK_SKELETON.bin", "SPAK");
			Manager::g_animationManager.loadClipCollection("SPAK", "SPAK", "../Assets/SPAKFOLDER", Manager::g_animationManager.getSkeleton("SPAK"));
			tempLever = new Lever(i, propsToPlace.props[i].linkedItem,propsToPlace.props[i].isTrigger);
			tempLever->Init(propsToPlace.props[i].transform_position[0],
				propsToPlace.props[i].transform_position[1],
				propsToPlace.props[i].transform_position[2],
				propsToPlace.props[i].transform_rotation[0],
				propsToPlace.props[i].transform_rotation[1],
				propsToPlace.props[i].transform_rotation[2]);
			triggerHandler->Triggers.push_back(tempLever);
			triggerHandler->netWorkTriggers.insert(std::pair<int, Trigger*>(i, tempLever));
			tempLever = nullptr;
			break;
		case(5):
			Manager::g_meshManager.loadDynamicMesh("BARS");
			Manager::g_textureManager.loadTextures("BARS");
			Manager::g_animationManager.loadSkeleton("../Assets/BARSFOLDER/BARS_SKELETON.bin", "BARS");
			Manager::g_animationManager.loadClipCollection("BARS", "BARS", "../Assets/BARSFOLDER", Manager::g_animationManager.getSkeleton("BARS"));
			tempBars = new Bars(i, propsToPlace.props[i].linkedItem, propsToPlace.props[i].isTrigger);
			tempBars->Init(propsToPlace.props[i].transform_position[0],
				propsToPlace.props[i].transform_position[1],
				propsToPlace.props[i].transform_position[2],
				propsToPlace.props[i].transform_rotation[0],
				propsToPlace.props[i].transform_rotation[1],
				propsToPlace.props[i].transform_rotation[2],
				propsToPlace.props[i].BBOX_INFO[0],
				propsToPlace.props[i].BBOX_INFO[1],
				propsToPlace.props[i].BBOX_INFO[2],
				propsToPlace.props[i].transform_scale[0], 
				propsToPlace.props[i].transform_scale[1],
				propsToPlace.props[i].transform_scale[2]);
			triggerHandler->Triggerables.push_back(tempBars);
			tempDoor = nullptr;
			break;
		default:
			break;
		}
	}


	triggerHandler->LoadTriggerPairMap();
	
}

Room::Room(const short unsigned int roomIndex, b3World * worldPtr)
{
	this->m_roomIndex = roomIndex;
	this->m_worldPtr = worldPtr;

	m_grid = nullptr;
	m_pathfindingGrid = new Grid();
}
Room::Room(const short unsigned int roomIndex, b3World * worldPtr, int arrayIndex, Player *  playerPtr) : HUDComponent()
{
	std::string filePath = "RUM";
	filePath += std::to_string(roomIndex);
	this->m_arrayIndex = arrayIndex;
	this->m_roomIndex = roomIndex;
	this->m_playerInRoomPtr = playerPtr;

	this->m_worldPtr = worldPtr;
	setAssetFilePath(filePath);
	triggerHandler = new TriggerHandler();
	triggerHandler->RegisterThisInstanceToNetwork();

	m_lose = new Quad();
	m_lose->init();
	m_lose->setPosition(0.5f, 0.5f);
	m_lose->setScale(0.5f, 0.25f);
	
	m_lose->setString("YOU LOST");
	m_lose->setUnpressedTexture("SPHERE");
	m_lose->setPressedTexture("DAB");
	m_lose->setHoverTexture("PIRASRUM");
	m_lose->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_lose->setFont(FontHandler::getFont("consolas32"));

	HUDComponent::AddQuad(m_lose);

	m_grid = nullptr;
	m_pathfindingGrid = new Grid();
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

const std::vector<Enemy*>* Room::getEnemies() const
{
	return &m_roomGuards;
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
		delete m_grid->gridPoints;
		delete m_grid;
		m_roomLoaded = false;
	}
}

void Room::LoadRoomToMemory()
{
	//TODO:: add all the assets to whatever
	if (m_roomLoaded == false)
	{
		ImporterLibrary::CustomFileLoader fileLoader;
		ImporterLibrary::PointLights tempLights = fileLoader.readLightFile(this->getAssetFilePath());
		for (int i = 0; i < tempLights.nrOf; i++)
		{
			this->m_pointLights.push_back(new PointLight(tempLights.lights[i].translate, tempLights.lights[i].color, tempLights.lights[i].intensity));
		}
		delete tempLights.lights;

		if (m_grid)
			delete m_grid;
		m_grid = fileLoader.readGridFile(this->getAssetFilePath());
		m_pathfindingGrid->CreateGridWithWorldPosValues(*m_grid);

		m_roomLoaded = true;

		ImporterLibrary::StartingPos player1Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 1);
		ImporterLibrary::StartingPos player2Start = fileLoader.readPlayerStartFile(this->getAssetFilePath(), 2);

		m_player1StartPos = DirectX::XMFLOAT4(player1Start.startingPos[0], player1Start.startingPos[1], player1Start.startingPos[2], 1.0f);
		m_player2StartPos = DirectX::XMFLOAT4(player2Start.startingPos[0], player2Start.startingPos[1], player2Start.startingPos[2], 1.0f);

		ImporterLibrary::GuardStartingPositions tempGuards = fileLoader.readGuardStartFiles(this->getAssetFilePath());

		ImporterLibrary::PropItemToEngine tempProps = fileLoader.readPropsFile(this->getAssetFilePath());
		
		placeRoomProps(tempProps);
		delete tempProps.props;

		for (int i = 0; i < tempGuards.nrOf; i++)
		{
			this->m_roomGuards.push_back(new Enemy(m_worldPtr, tempGuards.startingPositions[i].startingPos[0], tempGuards.startingPositions[i].startingPos[1], tempGuards.startingPositions[i].startingPos[2]));

			/*std::vector<Node*> path = m_pathfindingGrid.FindPath(Tile(0, 0), Tile(24, 13));
			this->m_roomGuards.at(i)->SetPathVector(path);*/
			//this->m_roomGuards.at(i)->setPosition(-10, 0, -10);
		}
		delete tempGuards.startingPositions;

		/*
			Test paths for guard on bottom floor
		*/
		// 0, 0 -> 5, 0
		std::vector<Node*> fullPath = m_pathfindingGrid->FindPath(Tile(0, 0), Tile(5, 0));
		// 5, 0 -> 5, 10
		std::vector<Node*> partOfPath = m_pathfindingGrid->FindPath(Tile(5, 0), Tile(5, 10));
		fullPath.insert(std::end(fullPath), std::begin(partOfPath), std::end(partOfPath));
		partOfPath.clear();
		// 5, 10 -> 0, 10
		partOfPath = m_pathfindingGrid->FindPath(Tile(5, 10), Tile(0, 10));
		fullPath.insert(std::end(fullPath), std::begin(partOfPath), std::end(partOfPath));
		partOfPath.clear();
		// 0, 10 -> 0, 0
		partOfPath = m_pathfindingGrid->FindPath(Tile(0, 10), Tile(0, 0));
		fullPath.insert(std::end(fullPath), std::begin(partOfPath), std::end(partOfPath));
		partOfPath.clear();

		m_roomGuards.at(1)->SetPathVector(fullPath);

		//getPath();


		StaticAsset * temp = new StaticAsset();
		temp->Init(*m_worldPtr, 1, 1, 1);
		//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
		temp->setPosition(0, 0, 0);
		Manager::g_meshManager.loadStaticMesh(this->getAssetFilePath());
		temp->setTexture(Manager::g_textureManager.getTexture(this->getAssetFilePath()));
		temp->setModel(Manager::g_meshManager.getStaticMesh(this->getAssetFilePath()));


		CollisionBoxes = new BaseActor();
		ImporterLibrary::CollisionBoxes boxes = Manager::g_meshManager.getCollisionBoxes(this->getAssetFilePath());
		CollisionBoxes->Init(*m_worldPtr, boxes);
		
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
	}


	for (auto light : m_pointLights)
	{
		light->setColor(200, 102, 50);
	}
}

void Room::getPath()
{
	Tile t = m_pathfindingGrid->WorldPosToTile(m_playerInRoomPtr->getPosition().x, m_playerInRoomPtr->getPosition().z);
	if (t.getX() != -1)
	{
		for (int i = 0; i < m_roomGuards.size(); i++)
		{
			DirectX::XMFLOAT4A pos = m_roomGuards.at(i)->getPosition();
			Tile tile = m_pathfindingGrid->WorldPosToTile(pos.x, pos.z);
			m_roomGuards.at(i)->SetAlertVector(m_pathfindingGrid->FindPath(tile,
				m_pathfindingGrid->WorldPosToTile(m_playerInRoomPtr->getPosition().x, m_playerInRoomPtr->getPosition().z)));
		}
	}
}

void Room::Update(float deltaTime)
{
	for (size_t i = 0; i < m_roomGuards.size(); i++)
	{
		this->m_roomGuards.at(i)->Update(deltaTime);
		this->m_roomGuards.at(i)->CullingForVisability(*m_playerInRoomPtr->getTransform());
		this->m_roomGuards.at(i)->QueueForVisibility();
		this->m_roomGuards.at(i)->GetDirectionToPlayer(m_playerInRoomPtr->getPosition(), *m_playerInRoomPtr->getCamera());
		this->m_roomGuards.at(i)->_IsInSight();
		this->m_roomGuards.at(i)->PhysicsUpdate(deltaTime);
		vis.push_back(this->m_roomGuards.at(i)->getPlayerVisibility());

	}
	int endvis = 0;
	
	m_playerInRoomPtr->setEnemyPositions(this->m_roomGuards);

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
	for (unsigned int i = 0; i < m_roomGuards.size(); ++i)
	{
		if (m_roomGuards.at(i)->getIfLost() == true)
		{
			m_youLost = true;
			break;
		}
	}
	if (m_youLost)
	{
#if _DEBUG
		PlayState::setYouLost(false);
#else
		PlayState::setYouLost(true);
#endif

		HUDComponent::HUDUpdate(deltaTime);
	}

	if (m_playerInRoomPtr->getPosition().y <= -50)
	{
		m_playerInRoomPtr->setPosition(m_player1StartPos.x, m_player1StartPos.y + 1, m_player1StartPos.z);
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
	
	triggerHandler->Draw();

	if (m_youLost)
	{
		HUDComponent::HUDDraw();
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
		CollisionBoxes->Release(*RipExtern::g_world);
		delete CollisionBoxes;
		CollisionBoxes = nullptr;
	}
	for (auto light : m_pointLights)
	{
		delete light;
	}
	m_pointLights.clear();
	for (auto enemy : m_roomGuards)
	{
		delete enemy;
	}
	for (auto ab : m_audioBoxes)
	{
		ab->release();
	}
	delete m_grid->gridPoints;

	delete m_grid;

	triggerHandler->Release();
	delete triggerHandler;
	delete m_pathfindingGrid;
}

void Room::loadTextures()
{
	Manager::g_textureManager.loadTextures(this->getAssetFilePath());
}
