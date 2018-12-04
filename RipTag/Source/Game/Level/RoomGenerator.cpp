#include "RipTagPCH.h"
#include "RoomGenerator.h"

RoomGenerator::RoomGenerator()
{	
}

RoomGenerator::~RoomGenerator()
{
}

void RoomGenerator::dbgFuncSpawnAboveMap()
{
	asset = DBG_NEW BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth * 2, 5, m_roomDepth * 2);
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 10, 0);
	m_generated_assetVector.push_back(asset);
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 15, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 15, 0, 1));
}

void RoomGenerator::applyTransformationToBoundingBox(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes & boxesToModify)
{
	DirectX::XMVECTOR decomposeTranslation;
	DirectX::XMVECTOR decomposeRotation;
	DirectX::XMVECTOR decomposeScaling;
	for (unsigned int k = 0; k < boxesToModify.nrOfBoxes && true; k++)
	{
		DirectX::XMFLOAT3 cPos = DirectX::XMFLOAT3(boxesToModify.boxes[k].translation);
		DirectX::XMFLOAT3 cScl = DirectX::XMFLOAT3(boxesToModify.boxes[k].scale);
		DirectX::XMFLOAT4 cQuat = DirectX::XMFLOAT4(boxesToModify.boxes[k].rotation);

		DirectX::XMMATRIX boxTranslationMatrix = DirectX::XMMatrixTranslation(cPos.x, cPos.y, cPos.z);
		DirectX::XMMATRIX boxRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&cQuat));
		DirectX::XMMATRIX boxScaleMatrix = DirectX::XMMatrixScaling(cScl.x, cScl.y, cScl.z);

		DirectX::XMMATRIX itemWorldSpace = boxScaleMatrix * boxRotationMatrix * boxTranslationMatrix;
		itemWorldSpace = itemWorldSpace * roomMatrix;

		DirectX::XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);

		boxesToModify.boxes[k].translation[0] = DirectX::XMVectorGetX(decomposeTranslation);
		boxesToModify.boxes[k].translation[1] = DirectX::XMVectorGetY(decomposeTranslation);
		boxesToModify.boxes[k].translation[2] = DirectX::XMVectorGetZ(decomposeTranslation);

		boxesToModify.boxes[k].rotation[0] = DirectX::XMVectorGetX(decomposeRotation);
		boxesToModify.boxes[k].rotation[1] = DirectX::XMVectorGetY(decomposeRotation);
		boxesToModify.boxes[k].rotation[2] = DirectX::XMVectorGetZ(decomposeRotation);
		boxesToModify.boxes[k].rotation[3] = DirectX::XMVectorGetW(decomposeRotation);

		boxesToModify.boxes[k].scale[0] = DirectX::XMVectorGetX(decomposeScaling);
		boxesToModify.boxes[k].scale[1] = DirectX::XMVectorGetY(decomposeScaling);
		boxesToModify.boxes[k].scale[2] = DirectX::XMVectorGetZ(decomposeScaling);

		
	}
}

void RoomGenerator::_generateGrid()
{
	int iterationsDepth = m_roomDepth * 2 + 1;
	int iterationsWidth = m_roomWidth * 2 + 1;
	Manager::g_textureManager.loadTextures("RED");
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	DirectX::BoundingBox testBox;

	std::cout << green << "Generating grid with" << red << " Box Intersection" << white << std::endl;
	for (int i = 0; i < iterationsDepth; i++)
	{
		for (int j = 0; j < iterationsWidth; j++)
		{
			bool col = false;
			int index = j + i * iterationsWidth;
			Node node = m_generatedGrid->GetWorldPosFromIndex(index);
			bool placed = false;
			testBox = DirectX::BoundingBox(DirectX::XMFLOAT3(node.worldPos.x, 1.0f, node.worldPos.y), DirectX::XMFLOAT3(0.5, 1, 0.5));
			for (int x = 0; x < m_generated_boundingBoxes.size() && !col; x++)
			{
				if (m_generated_boundingBoxes[x]->Intersects(testBox))
				{
					m_generatedGrid->BlockGridTile(index, false);
					break;
				}
			}
		}
	}

	
	/*std::ofstream lol;
	lol.open(" map_noBlockAlg.txt");
	for (int i = 0; i < iterationsDepth; i++)
	{
		for (int j = 0; j < iterationsWidth; j++)
		{
			int index = i + j * iterationsWidth;
			Node node = m_generatedGrid->GetWorldPosFromIndex(index);
			if (node.tile.getPathable())
				lol << " ";
			else
				lol << "#";
			lol << " ";
		}
		lol << "\n";
	}*/
	std::cout << green << "Blocking unpathable tiles with" << red << " Recursive stuff happening" << white << std::endl;
	for (int i = 0; i < iterationsDepth; i++)
	{
		for (int j = 0; j < iterationsWidth; j++)
		{
			m_generatedGrid->BlockIfNotPathable(j, i);
		}
	}
	/*lol.open("map_BlockAlg.txt");
	for (int i = 0; i < iterationsDepth; i++)
	{
		for (int j = 0; j < iterationsWidth; j++)
		{
			int index = i + j * iterationsWidth;
			Node node = m_generatedGrid->GetWorldPosFromIndex(index);
			if (node.tile.getPathable())
				lol << " ";
			else
				lol << "#";
			lol << " ";
		}
		lol << "\n";
	}
	lol.close();*/
}

void RoomGenerator::_makeFloor()
{
	asset = DBG_NEW BaseActor();
	m_roomDepth = (m_incrementalValueY * m_roomGridDepth) / 2.0f;
	m_roomWidth = (m_incrementalValueX * m_roomGridWidth) / 2.0f;
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth * 2, 0.5, m_roomDepth * 2);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 0, 0);
	asset->setScale(m_roomWidth * 2, 1, m_roomDepth * 2);
	asset->setObjectTag("FLOOOOOOR");
	m_generated_assetVector.push_back(asset);


	DirectX::XMFLOAT4 xmQ = { 0, 0, 0, 0 };
	DirectX::XMFLOAT4 xmPos = { 0, 0, 0, 1 };
	DirectX::XMFLOAT4 xmScl = { m_roomWidth * 2, 1, m_roomDepth * 2, 1 };
	FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
	ge->setActive(true);

	returnableRoom->getAudioBoxesVector()->push_back(ge);
}

void RoomGenerator::_makeRoof()
{
	asset = DBG_NEW BaseActor();
	m_roomDepth = (m_incrementalValueY * m_roomGridDepth) / 2.0f;
	m_roomWidth = (m_incrementalValueX * m_roomGridWidth) / 2.0f;
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth * 2, 0.5, m_roomDepth * 2);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 5.5, 0);
	asset->setScale(m_roomWidth * 2, 1, m_roomDepth * 2);
	asset->setObjectTag("FLOOOOOOR");
	m_generated_assetVector.push_back(asset);

	DirectX::XMFLOAT4 xmQ = { 0, 0, 0, 0 };
	DirectX::XMFLOAT4 xmPos = { 0, 5.5, 0, 1 };
	DirectX::XMFLOAT4 xmScl = { m_roomWidth * 2, 1, m_roomDepth * 2, 1 };
	FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
	ge->setActive(true);

	returnableRoom->getAudioBoxesVector()->push_back(ge);
}

void RoomGenerator::_createEntireWorld()
{
	m_roomDepth = (m_incrementalValueY * m_roomGridDepth) / 2.0f;
	m_roomWidth = (m_incrementalValueX * m_roomGridWidth) / 2.0f;

	//std::vector<ImporterLibrary::GridStruct*> appendedGridStruct; 
	bool isRotated = false;
	int RANDOM_MOD_NR = 0;
	int MAX_SMALL_MODS = 11; //8
	int MAX_LARGE_MODS = 6; // 5
	bool * alreadyPickedSmallMods = DBG_NEW bool[MAX_SMALL_MODS];
	bool * alreadyPickedLargeMods = DBG_NEW bool[MAX_LARGE_MODS];
	for (int i = 0; i < MAX_SMALL_MODS; i++)
		alreadyPickedSmallMods[i] = false;
	for (int i = 0; i < MAX_LARGE_MODS; i++)
		alreadyPickedLargeMods[i] = false;

	RandomRoomGrid randomizer;
	ImporterLibrary::CustomFileLoader loader;
	randomizer.GenerateRoomLayout();

	int iterationsDepth = m_roomDepth * 2 + 1;
	int iterationsWidth = m_roomWidth * 2 + 1;

	m_generatedGrid = DBG_NEW Grid(-m_roomWidth, -m_roomDepth, iterationsWidth, iterationsDepth);
	m_generatedGrid->GenerateRoomNodeMap(&randomizer);

	int widthCounter = 0;
	int depthCounter = 0;
	ImporterLibrary::CollisionBoxes  modCollisionBoxes;
	modCollisionBoxes.boxes = nullptr;
	m_nrOfLevers = 0;

	Manager::g_meshManager.loadStaticMesh("CLOSEDWALL");
	Manager::g_meshManager.loadStaticMesh("OPENWALL");

	int depth = (int)(m_roomDepth + 0.5f);
	int width = (int)(m_roomWidth + 0.5f);
	int winRoomIndexPlacement = rand () % 5;
	bool isWinRoom = false;
	bool isStartRoom = false;
	winRoomIndexPlacement *= m_roomGridWidth;
	
	if (randomizer.m_rooms[winRoomIndexPlacement].type != 2)
	{
		for (int i = 0; i < m_roomGridWidth; i++)
		{
			if (randomizer.m_rooms[i * m_roomGridWidth].type == 2)
			{
				winRoomIndexPlacement = i * m_roomGridWidth;
				break;
			}
		}
	}
	
	for (int i = -depth + 10; i <= depth - 10; i += m_incrementalValueY)
	{
		widthCounter = 0;
		for (int j = -width + 10; j <= width - 10; j += m_incrementalValueX)
		{
			bool directions[4];
			int index = depthCounter * m_roomGridWidth + widthCounter;
			directions[0] = randomizer.m_rooms[index].north;
			directions[1] = randomizer.m_rooms[index].east;
			directions[2] = randomizer.m_rooms[index].south;
			directions[3] = randomizer.m_rooms[index].west;
			std::string MODNAMESTRING = "MOD" + std::to_string(RANDOM_MOD_NR);
			if (randomizer.m_rooms[index].type == 2)
			{
				RANDOM_MOD_NR = rand() % MAX_SMALL_MODS;
				if (alreadyPickedSmallMods[RANDOM_MOD_NR])
					RANDOM_MOD_NR = rand() % MAX_SMALL_MODS;
				alreadyPickedSmallMods[RANDOM_MOD_NR] = true;
				// To pick the correct module from the assets
				RANDOM_MOD_NR++;
			}
			else
			{
				RANDOM_MOD_NR = rand() % MAX_LARGE_MODS;
				MODNAMESTRING = "STORMOD" + std::to_string(RANDOM_MOD_NR);
				if (alreadyPickedLargeMods[RANDOM_MOD_NR])
					RANDOM_MOD_NR = rand() % MAX_LARGE_MODS;
				alreadyPickedLargeMods[RANDOM_MOD_NR] = true;
			}

			isRotated = false;
			if (randomizer.m_rooms[index].type == 1)
				isRotated = true;

			int BigRoomAddX = 0;
			int BigRoomAddZ = 0;
			if (randomizer.m_rooms[index].type == 0 || randomizer.m_rooms[index].type == 1)
			{
				if (!isRotated)
					BigRoomAddX = 10;
				else
					BigRoomAddZ = 10;
			}
			
			bool placeWall = true;
			isWinRoom = false;
			isStartRoom = false;
			if (randomizer.m_rooms[index].type == WIN_ROOM)
			{
				isWinRoom = true;
				MODNAMESTRING = "MOD" + std::to_string(99);
			}
			else if (randomizer.m_rooms[index].type == START_ROOM)
			{
				MODNAMESTRING = "MOD" + std::to_string(98);
				isStartRoom = true;
			}
			else if (randomizer.m_rooms[index].type == START_ROOM)
			{
				isStartRoom = true;
				MODNAMESTRING = "MOD" + std::to_string(66);
			}
#pragma region Guards
			Manager::g_meshManager.loadSkinnedMesh("STATE");
			Manager::g_meshManager.loadSkinnedMesh("GUARD");
			Manager::g_textureManager.loadTextures("SPHERE");
			ImporterLibrary::GuardStartingPositions tempGuards = loader.readGuardStartFiles(MODNAMESTRING);
			for (int k = 0; k < tempGuards.nrOf; k++)
			{
				if (isRotated == true)
				{
					float tempPosX = tempGuards.startingPositions[k].startingPos[0];
					tempGuards.startingPositions[k].startingPos[0] = j + tempGuards.startingPositions[k].startingPos[2];
					tempGuards.startingPositions[k].startingPos[2] = i + 10 + tempPosX;
				}
				else
				{
					tempGuards.startingPositions[k].startingPos[0] = j + tempGuards.startingPositions[k].startingPos[0] + BigRoomAddX;
					tempGuards.startingPositions[k].startingPos[2] = i + tempGuards.startingPositions[k].startingPos[2] + BigRoomAddZ;
				}

				Enemy * e = DBG_NEW Enemy(m_worldPtr, m_generatedRoomEnemies.size(), tempGuards.startingPositions[k].startingPos[0], tempGuards.startingPositions[k].startingPos[1], tempGuards.startingPositions[k].startingPos[2]);
				e->addTeleportAbility(*this->returnableRoom->getPLayerInRoomPtr()->getTeleportAbility());
				e->SetPlayerPointer(this->returnableRoom->getPLayerInRoomPtr());
				this->m_generatedRoomEnemies.push_back(e);
			}
			delete tempGuards.startingPositions;
#pragma endregion

#pragma region WALLS
			for (int x = 0; x < 4; x++)
			{
				placeWall = true;
				if (randomizer.m_rooms[index].leadsToRoom[x] != -1)
					if (randomizer.m_rooms[index].pairedWith == randomizer.m_rooms[index].leadsToRoom[x])
						placeWall = false;

				if (placeWall)
				{
					asset = DBG_NEW BaseActor();
					if (directions[x])
					{
						asset->setModel(Manager::g_meshManager.getStaticMesh("OPENWALL"));
						modCollisionBoxes = loader.readMeshCollisionBoxes("OPENWALL");
						asset->setTexture(Manager::g_textureManager.getTexture("OPENWALL"));
					}
					else
					{
						asset->setModel(Manager::g_meshManager.getStaticMesh("CLOSEDWALL"));
						modCollisionBoxes = loader.readMeshCollisionBoxes("CLOSEDWALL");
						asset->setTexture(Manager::g_textureManager.getTexture("CLOSEDWALL"));
					}

					if (x == 0)
					{
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*RipExtern::g_world, modCollisionBoxes);
						asset->setPosition(j, 2.5, i - 10.f);
						asset->setObjectTag("WALL");
						asset->setUserDataBody(asset);
						m_generated_assetVector.push_back(asset);
						for (int h = 0; h < (int)modCollisionBoxes.nrOfBoxes; h++)
						{
							float xPos = modCollisionBoxes.boxes[h].translation[0] + j;
							float yPos = modCollisionBoxes.boxes[h].translation[1] + 2.5;
							float zPos = modCollisionBoxes.boxes[h].translation[2] + i-10;
							DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].scale[0] * 0.5, modCollisionBoxes.boxes[h].scale[1] * 0.5, modCollisionBoxes.boxes[h].scale[2] * 0.5));
							m_generated_boundingBoxes.push_back(bb);

							float * f4Rot = modCollisionBoxes.boxes[h].rotation;
							float * f3Scl = modCollisionBoxes.boxes[h].scale;
							DirectX::XMFLOAT4 xmQ = { f4Rot[0], f4Rot[1], f4Rot[2], f4Rot[3] };
							DirectX::XMFLOAT4 xmPos = { xPos, yPos, zPos, 1 };
							DirectX::XMFLOAT4 xmScl = { f3Scl[0] * 0.5f, f3Scl[1] * 0.5f, f3Scl[2] * 0.5f, 1 };
							FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
							ge->setActive(true);

							returnableRoom->getAudioBoxesVector()->push_back(ge);
						}
					}
					else if (x == 1)
					{
						asset->setRotation(0, 90, 0, false);
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*RipExtern::g_world, modCollisionBoxes);
						asset->setPosition(j + 10.f, 2.5, i);
						asset->setObjectTag("WALL");
						m_generated_assetVector.push_back(asset);
						roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						for (int h = 0; h < (int)modCollisionBoxes.nrOfBoxes; h++)
						{
							float xPos = modCollisionBoxes.boxes[h].translation[0] + j + 10;
							float yPos = modCollisionBoxes.boxes[h].translation[1] + 2.5;
							float zPos = modCollisionBoxes.boxes[h].translation[2] + i;
							DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].scale[2] * 0.5, modCollisionBoxes.boxes[h].scale[1] * 0.5, modCollisionBoxes.boxes[h].scale[0] * 0.5));
							m_generated_boundingBoxes.push_back(bb);

							float * f4Rot = modCollisionBoxes.boxes[h].rotation;
							float * f3Scl = modCollisionBoxes.boxes[h].scale;
							DirectX::XMFLOAT4 xmQ = { f4Rot[0], f4Rot[1], f4Rot[2], f4Rot[3] };
							DirectX::XMFLOAT4 xmPos = { xPos, yPos, zPos, 1 };
							DirectX::XMFLOAT4 xmScl = { f3Scl[0] * 0.5f, f3Scl[1] * 0.5f, f3Scl[2] * 0.5f, 1 };
							FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
							ge->setActive(true);

							returnableRoom->getAudioBoxesVector()->push_back(ge);
						}
					}
					else if (x == 2)
					{
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
						asset->Init(*RipExtern::g_world, modCollisionBoxes);
						asset->setPosition(j, 2.5, i + 10.f);
						asset->setObjectTag("WALL");
						m_generated_assetVector.push_back(asset);
						roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						for (int h = 0; h < (int)modCollisionBoxes.nrOfBoxes; h++)
						{
							float xPos = modCollisionBoxes.boxes[h].translation[0] + j;
							float yPos = modCollisionBoxes.boxes[h].translation[1] + 2.5;
							float zPos = modCollisionBoxes.boxes[h].translation[2] + i + 10.f;
							DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].scale[0] * 0.5, modCollisionBoxes.boxes[h].scale[1] * 0.5, modCollisionBoxes.boxes[h].scale[2] * 0.5));
							m_generated_boundingBoxes.push_back(bb);

							float * f4Rot = modCollisionBoxes.boxes[h].rotation;
							float * f3Scl = modCollisionBoxes.boxes[h].scale;
							DirectX::XMFLOAT4 xmQ = { f4Rot[0], f4Rot[1], f4Rot[2], f4Rot[3] };
							DirectX::XMFLOAT4 xmPos = { xPos, yPos, zPos, 1 };
							DirectX::XMFLOAT4 xmScl = { f3Scl[0] * 0.5f, f3Scl[1] * 0.5f, f3Scl[2] * 0.5f, 1 };
							FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
							ge->setActive(true);

							returnableRoom->getAudioBoxesVector()->push_back(ge);
						}
					}
					else if (x == 3)
					{
						asset->setRotation(0, 90, 0, false);
						DirectX::XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						roomSpace = DirectX::XMMatrixTranspose(roomSpace);
						asset->setObjectTag("WALL");
						roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
						if (isWinRoom)
						{
							if (modCollisionBoxes.boxes)
								delete[] modCollisionBoxes.boxes;
							asset->setModel(Manager::g_meshManager.getStaticMesh("OPENWALL"));
							modCollisionBoxes = loader.readMeshCollisionBoxes("OPENWALL");
							asset->setTexture(Manager::g_textureManager.getTexture("OPENWALL"));
							applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
							asset->Init(*RipExtern::g_world, modCollisionBoxes);
							asset->setPosition(j - 10.f, 2.5, i);
						}
						else
						{
							applyTransformationToBoundingBox(roomSpace, modCollisionBoxes);
							asset->Init(*RipExtern::g_world, modCollisionBoxes);
							asset->setPosition(j - 10.f, 2.5, i);
						}
						m_generated_assetVector.push_back(asset);
						for (int h = 0; h < (int)modCollisionBoxes.nrOfBoxes; h++)
						{
							float xPos = modCollisionBoxes.boxes[h].translation[0] + j - 10;
							float yPos = modCollisionBoxes.boxes[h].translation[1] + 2.5;
							float zPos = modCollisionBoxes.boxes[h].translation[2] + i;
							DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].scale[2] * 0.5, modCollisionBoxes.boxes[h].scale[1] * 0.5, modCollisionBoxes.boxes[h].scale[0] * 0.5));
							m_generated_boundingBoxes.push_back(bb);

							float * f4Rot = modCollisionBoxes.boxes[h].rotation;
							float * f3Scl = modCollisionBoxes.boxes[h].scale;
							DirectX::XMFLOAT4 xmQ = { f4Rot[0], f4Rot[1], f4Rot[2], f4Rot[3] };
							DirectX::XMFLOAT4 xmPos = { xPos, yPos, zPos, 1 };
							DirectX::XMFLOAT4 xmScl = { f3Scl[0] * 0.5f, f3Scl[1] * 0.5f, f3Scl[2] * 0.5f, 1 };
							FMOD::Geometry * ge = AudioEngine::CreateCube(0.05f, 0.03f, xmPos, xmScl, xmQ);
							ge->setActive(true);
							returnableRoom->getAudioBoxesVector()->push_back(ge);
						}
					}
					if (modCollisionBoxes.boxes)
						delete[] modCollisionBoxes.boxes;
				}
			}
			if (isStartRoom)
			{
				returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(j +2, 2, i, 1));
				returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(j, 2, i, 1));
			}
#pragma endregion
//Above 10 mB

			
			returnableRoom->getreverbVector()->push_back(AudioEngine::CreateReverb(FMOD_VECTOR{ (float)i, 2.5, (float)j }, 3, 10));
#pragma region LIGHTS
			if (!randomizer.m_rooms[index].propsPlaced && !isStartRoom)
			{
				ImporterLibrary::PointLights tempLights = loader.readLightFile(MODNAMESTRING);
				PointLight * tempLight;
				for (int k = 0; k < tempLights.nrOf; k++)
				{
					if (isRotated == true)
					{
						float tempPosX = tempLights.lights[k].translate[0];
						tempLights.lights[k].translate[0] = j + tempLights.lights[k].translate[2];
						tempLights.lights[k].translate[2] = i + tempPosX;
					}
					else
					{
						tempLights.lights[k].translate[0] = j + tempLights.lights[k].translate[0] + BigRoomAddX;
						tempLights.lights[k].translate[2] = i + tempLights.lights[k].translate[2] + BigRoomAddZ;
					}
					if (tempLights.lights[k].intensity < 10)
						tempLights.lights[k].intensity = 10;

					tempLight = DBG_NEW PointLight(tempLights.lights[k].translate, tempLights.lights[k].color, tempLights.lights[k].intensity);
					tempLight->setColor(250.0f, 172.0f, 100.f);

					ParticleEmitter * particleEmitter;
					particleEmitter = DBG_NEW ParticleEmitter();
					particleEmitter->setPosition(tempLights.lights[k].translate[0], tempLights.lights[k].translate[1], tempLights.lights[k].translate[2], 0);

					Torch * tempTorch = DBG_NEW Torch(tempLight, particleEmitter, returnableRoom->getTriggerHandler()->netWorkTriggers.size());
					m_generatedTorches.push_back(tempTorch);
					returnableRoom->getTriggerHandler()->netWorkTriggers.insert(std::pair<int, Trigger*>(returnableRoom->getTriggerHandler()->netWorkTriggers.size(), m_generatedTorches.back()));
				}
				delete tempLights.lights;

				for (auto & e : m_generatedRoomEnemies)
				{
					e->SetTorchContainer(m_generatedTorches);
				}
			}
#pragma endregion

#pragma region PROPS
			if (!randomizer.m_rooms[index].propsPlaced && !isStartRoom)
			{
				ImporterLibrary::PropItemToEngine tempProps = loader.readPropsFile(MODNAMESTRING); 
				
				for (int k = 0; k < tempProps.nrOfItems; k++)
				{
					if (tempProps.props[k].typeOfProp == 4)
						m_nrOfLevers++;
					if (isRotated == true)
					{
						float tempPosX = tempProps.props[k].transform_position[0];
						tempProps.props[k].transform_rotation[1] += 90.f;
						tempProps.props[k].transform_position[0] = j + tempProps.props[k].transform_position[2];
						tempProps.props[k].transform_position[2] = i + 10 + tempPosX;

					}
					else
					{
						tempProps.props[k].transform_position[0] = j + tempProps.props[k].transform_position[0] + BigRoomAddX;
						tempProps.props[k].transform_position[2] = i + tempProps.props[k].transform_position[2] + BigRoomAddZ;
					}
					if(tempProps.props[k].typeOfProp != 11 
						&& tempProps.props[k].typeOfProp != 16)
						_modifyPropBoundingBoxes(tempProps.props[k]);
				}
			
				returnableRoom->addPropsAndAssets(tempProps, returnableRoom->getTriggerHandler(), &m_generated_assetVector, true);
				delete[] tempProps.props;
				randomizer.m_rooms[index].propsPlaced = true;
				if (randomizer.m_rooms[index].type == 0 || randomizer.m_rooms[index].type == 1)
					randomizer.m_rooms[randomizer.m_rooms[index].pairedWith].propsPlaced = true;
			}
#pragma endregion		

#pragma region ColissionBoxes
				asset = DBG_NEW BaseActor();
				ImporterLibrary::CollisionBoxes boxes = loader.readMeshCollisionBoxes(MODNAMESTRING);
				for (unsigned int k = 0; k < boxes.nrOfBoxes; k++)
				{
					float xPos = boxes.boxes[k].translation[0] + j - 10;
					float yPos = boxes.boxes[k].translation[1] + 2.5;
					float zPos = boxes.boxes[k].translation[2] + i;
					DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(xPos, yPos, zPos), DirectX::XMFLOAT3(boxes.boxes[k].scale[2] * 0.5, boxes.boxes[k].scale[1] * 0.5, boxes.boxes[k].scale[0] * 0.5));
					boxes.boxes[k].translation[0] = xPos;
					boxes.boxes[k].translation[1] = yPos;
					boxes.boxes[k].translation[2] = zPos;
					m_generated_boundingBoxes.push_back(bb);
				}
				asset->Init(*m_worldPtr, boxes);
				m_generated_assetVector.push_back(asset);
				delete [] boxes.boxes;
#pragma endregion
			widthCounter++;
		}
		depthCounter++;
	}
	if (alreadyPickedSmallMods)
		delete [] alreadyPickedSmallMods;
	alreadyPickedSmallMods = nullptr;
	if (alreadyPickedLargeMods)
		delete [] alreadyPickedLargeMods;
	alreadyPickedLargeMods = nullptr;
}

void RoomGenerator::_generateGuardPaths()
{
	std::cout << green << "Generating guards path with" << red << " A* and some magic" << white << std::endl;
	for (int i = 0; i < m_generatedRoomEnemies.size(); i++)
	{
		Enemy * currentEnemey = m_generatedRoomEnemies[i];
		DirectX::XMFLOAT4A pos = currentEnemey->getPosition();
		// Set a path
		Tile enemyPos = m_generatedGrid->WorldPosToTile(pos.x, pos.z);
		bool gotDestination = false;
		int x = returnRandomInGridWidth();
		int z = returnRandomInGridDepth();
		Tile destination = m_generatedGrid->WorldPosToTile(x, z);
		while (!gotDestination)
		{
			if (destination.getPathable())
				gotDestination = true;
			else
			{
				x = returnRandomInGridWidth();
				z = returnRandomInGridDepth();
				destination = m_generatedGrid->WorldPosToTile(x, z);
			}
		}
		currentEnemey->SetPathVector(m_generatedGrid->FindPath(enemyPos, destination));
	}
}

int RoomGenerator::returnRandomInGridWidth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomWidth;
	float max = m_roomWidth - 1;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomInGridDepth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomDepth;
	float max = m_roomDepth - 1;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomBetween(int min, int max)
{
	float randomNr = (float)rand() / RAND_MAX;
	return (min + randomNr * (max - (min)));
}

void RoomGenerator::_modifyPropBoundingBoxes(ImporterLibrary::PropItem prop)
{
	DirectX::XMVECTOR translation, rotation, scale;


	translation = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(prop.transform_position));	

	rotation = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(
		DirectX::XMConvertToRadians(prop.transform_rotation[0]),
		DirectX::XMConvertToRadians(prop.transform_rotation[1]),
		DirectX::XMConvertToRadians(prop.transform_rotation[2])
	));

	rotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);

	float newScale[3];
	for (int i = 0; i < 3; i++)
	{
		newScale[i] = prop.BBOX_INFO[i] * prop.transform_scale[i];
	}
	scale = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(newScale));

	DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox(DirectX::XMFLOAT3(0,0,0), DirectX::XMFLOAT3(newScale[0], newScale[1], newScale[2]));
	bb->Transform(*bb, 1, rotation, translation);
	m_generated_boundingBoxes.push_back(bb);
}

void RoomGenerator::moveCheckBoxes(DirectX::XMFLOAT3 startPos, ImporterLibrary::CollisionBoxes & modCollisionBoxes)
{
	for (int h = 0; h < (int)modCollisionBoxes.nrOfBoxes; h++)
	{
		using namespace DirectX;
		DirectX::XMVECTOR translation, rotation, scale;
		translation = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].translation));
		translation = DirectX::XMVectorAdd(translation, DirectX::XMLoadFloat3(&startPos));
		rotation = DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(modCollisionBoxes.boxes[h].rotation));
		scale = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(modCollisionBoxes.boxes[h].scale));

		XMMATRIX matrixTranslation = XMMatrixTranslationFromVector(translation);
		XMMATRIX matrixRotation = XMMatrixRotationQuaternion(rotation);
		XMMATRIX matrixScale = XMMatrixScalingFromVector(scale);
		XMMATRIX worldMatrix = matrixScale * matrixRotation* matrixTranslation;
		//DirectX::XMMATRIX worldMatrix = DirectX::xmmatrixt;
		DirectX::BoundingBox * bb = DBG_NEW DirectX::BoundingBox;// = new DirectX::BoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(1, 1, 1));
		bb->Transform(*bb, worldMatrix);
		m_generated_boundingBoxes.push_back(bb);
	}
}

Room * RoomGenerator::getGeneratedRoom( b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	this->m_worldPtr = worldPtr;
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");
	Manager::g_meshManager.loadStaticMesh("WALL");
	Manager::g_textureManager.loadTextures("OPENWALL");
	Manager::g_textureManager.loadTextures("CLOSEDWALL");

	returnableRoom = DBG_NEW Room(worldPtr, arrayIndex, playerPtr);
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));

	_createEntireWorld();
	_generateGrid();
	_generateGuardPaths();
	_makeFloor();
	_makeRoof();
	
	returnableRoom->setGrid(this->m_generatedGrid);

	m_generatedRoomEnemyHandler = DBG_NEW EnemyHandler;
	m_generatedRoomEnemyHandler->Init(m_generatedRoomEnemies, playerPtr, this->m_generatedGrid);

	//dbgFuncSpawnAboveMap();

	returnableRoom->setTorches(m_generatedTorches);
	returnableRoom->setEnemyhandler(m_generatedRoomEnemyHandler);
	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setRoomGuards(m_generatedRoomEnemies);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	returnableRoom->loadTriggerPairMap();
	returnableRoom->setRoomIndex(-1);

	for (int i = 0; i < m_generated_boundingBoxes.size(); i++)
		delete m_generated_boundingBoxes[i];
	m_generated_boundingBoxes.clear();

	return returnableRoom;
}
