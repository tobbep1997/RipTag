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
			Manager::g_meshManager.loadSkinnedMesh("PLATE");
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
			Manager::g_meshManager.loadSkinnedMesh("DOOR");
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
			Manager::g_meshManager.loadSkinnedMesh("SPAK");
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
			Manager::g_meshManager.loadSkinnedMesh("BARS");
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
	m_pathfindingGrid = DBG_NEW Grid();
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
Room::Room(b3World * worldPtr, int arrayIndex, Player * playerPtr)
{
	//GeneratedRoom
	this->m_roomIndex = -1;
	this->m_playerInRoomPtr = playerPtr;
	this->m_arrayIndex = arrayIndex;
	this->m_worldPtr = worldPtr;

	triggerHandler = new TriggerHandler();
	triggerHandler->RegisterThisInstanceToNetwork();

	m_lose = new Quad();
	m_lose->init();
	m_lose->setPosition(0.5f, 0.5f);
	m_lose->setScale(0.5f, 0.25f);

	m_lose->setString("YOU LOST");
	m_lose->setUnpressedTexture("gui_transparent_pixel");
	m_lose->setPressedTexture("gui_transparent_pixel");
	m_lose->setHoverTexture("gui_transparent_pixel");
	m_lose->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
	m_lose->setFont(FontHandler::getFont("consolas16"));

	HUDComponent::AddQuad(m_lose);

	m_grid = nullptr;

}
Room::~Room()
{
}

void Room::setRoomIndex(const short unsigned int roomIndex)
{
	this->m_roomIndex = roomIndex;
}

short int Room::getRoomIndex()
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

void Room::GiveCameraToParticles(Camera * ptr)
{
	for (auto torch : m_Torches)
		torch->setCamera(ptr);
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
			PointLight * p_pointLight = nullptr;
			ParticleEmitter * p_emit = nullptr;

			p_pointLight = new PointLight(tempLights.lights[i].translate, tempLights.lights[i].color, tempLights.lights[i].intensity);
			p_pointLight->setColor(90, 112.0f, 130.0f);
			
			p_emit = new ParticleEmitter();
			p_emit->setPosition(tempLights.lights[i].translate[0], tempLights.lights[i].translate[1], tempLights.lights[i].translate[2]);

			m_Torches.push_back( new Torch(p_pointLight, p_emit, 0));
			
			FMOD_VECTOR at = { tempLights.lights[i].translate[0], tempLights.lights[i].translate[1],tempLights.lights[i].translate[2] };
			AudioEngine::PlaySoundEffect(RipSounds::g_torch, &at, AudioEngine::Other)->setVolume(0.5f);
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
			Enemy * e = DBG_NEW Enemy(m_worldPtr, tempGuards.startingPositions[i].startingPos[0], tempGuards.startingPositions[i].startingPos[1], tempGuards.startingPositions[i].startingPos[2]);
			e->addTeleportAbility(*this->m_playerInRoomPtr->getTeleportAbility());
			e->SetPlayerPointer(m_playerInRoomPtr);
			this->m_roomGuards.push_back(e);
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

		m_roomGuards.at(0)->SetPathVector(m_pathfindingGrid->FindPath(Tile(0, 0), Tile(0, 1)));

		//getPath();


		BaseActor * temp = DBG_NEW BaseActor();
		temp->Init(*m_worldPtr, e_staticBody, 1, 1, 1);
		//te->p.Init(*m_worldPtr, e_dynamicBody, 1.0f, 1.0f, 1.0f);
		temp->setPosition(0, 0, 0);
		Manager::g_meshManager.loadStaticMesh(this->getAssetFilePath());
		temp->setTexture(Manager::g_textureManager.getTexture(this->getAssetFilePath()));
		temp->setModel(Manager::g_meshManager.getStaticMesh(this->getAssetFilePath()));


		CollisionBoxes = DBG_NEW BaseActor();
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
	m_enemyHandler = DBG_NEW EnemyHandler();
	m_enemyHandler->Init(m_roomGuards, m_playerInRoomPtr, m_pathfindingGrid);

	int nrOfTriggers = triggerHandler->netWorkTriggers.size();
	for (int i = 0; i < m_Torches.size(); i++)
	{
		m_Torches[i]->setUniqueID(nrOfTriggers + i);
		triggerHandler->netWorkTriggers.insert(std::pair<int, Trigger*>(m_Torches[i]->getUniqueID(), m_Torches[i]));
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

void Room::Update(float deltaTime, Camera * camera)
{
	/*for (size_t i = 0; i < m_roomGuards.size(); i++)
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
	
	vis.clear();*/
	for (auto torch : m_Torches)
	{
		torch->Update(deltaTime);
		auto light = torch->getPointLightPtr();
		light->setDropOff(2.0425345f);
		light->setPower(2.0f);
		light->setIntensity(light->TourchEffect(deltaTime * .1f, 10.1f, 8.5f));
	}
	/*for (int i = 0; i < m_emitters.size(); i++)
	{
		m_emitters[i]->Update(deltaTime, camera);
	}*/
	m_playerInRoomPtr->setEnemyPositions(this->m_roomGuards);
	m_enemyHandler->Update(deltaTime);

	/*for (auto light : m_pointLights)
	{
		light->setDropOff(2.0425345f);
		light->setPower(2.0f);
		light->setIntensity(light->TourchEffect(deltaTime * .1f, 10.1f, 8.5f));
	}*/
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
	//for (auto light : m_pointLights)
	//{
	//	light->QueueLight();
	//}
	//for (auto torch : m_Torches)
	//{
	//	torch->QueueLight();
	//	//torch->Draw();
	//}

	for (size_t i = 0; i < m_roomGuards.size(); i++)
		this->m_roomGuards.at(i)->Draw();
	
	triggerHandler->Draw();

	if (m_youLost)
	{
		HUDComponent::HUDDraw();
	}

	for (auto guard : m_roomGuards)
	{
		guard->DrawGuardPath();
	}
	/*for (int i = 0; i < m_emitters.size(); i++)
	{
		m_emitters[i]->Queue();
	}*/

	for (auto torch : m_Torches)
	{
		torch->Draw();
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
	for (auto enemy : m_roomGuards)
	{
		delete enemy;
	}
	for (auto ab : m_audioBoxes)
	{
		ab->release();
	}
	if (m_grid)
	{
		delete m_grid->gridPoints;

		delete m_grid;
	}
	for (int i = 0; i < m_Torches.size(); i++)
	{
		/*delete m_Torches[i];
		m_Torches[i] = nullptr;*/
	}
	m_Torches.clear();


	triggerHandler->Release();
	delete triggerHandler;
	delete m_enemyHandler;
	delete m_pathfindingGrid;
}

void Room::loadTextures()
{
	Manager::g_textureManager.loadTextures(this->getAssetFilePath());
}
