#include "RipTagPCH.h"
#include "RoomGenerator.h"

RoomGenerator::RoomGenerator()
{
	
}


RoomGenerator::~RoomGenerator()
{

}

void RoomGenerator::_generateGrid()
{
	srand(time(NULL));

	m_nrOfWalls = 4; // MAke random
	m_roomDepth = (20 * m_roomGridDepth) / 2.0f;
	m_roomWidth = (15 * m_roomGridWidth) / 2.0f;
	m_generatedGrid = DBG_NEW Grid(-m_roomWidth, -m_roomDepth, m_roomWidth * 2, m_roomDepth * 2);
	
}

void RoomGenerator::_makeFloor()
{
	asset = new BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth, 0.5, m_roomDepth);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomWidth, m_roomDepth);
	asset->setPosition(0, 0, 0);
	
	asset->setScale(abs(m_roomWidth*2), 1, abs(m_roomDepth* 2));
	m_generated_assetVector.push_back(asset);
}

void RoomGenerator::_makeWalls()
{
	int m_roomDepth = this->m_roomDepth;
	int	m_roomWidth = this->m_roomWidth + 1;
	for (int i = 0; i < m_nrOfWalls; i++)
	{
		switch (i)
		{
		case(0):
			asset = DBG_NEW BaseActor();
			asset->setTextureTileMult(100, 10);
			asset->Init(*m_worldPtr, e_staticBody, 1, m_height / 2, m_roomDepth);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(m_roomWidth, m_height / 2, 0);
			asset->setScale(1, m_height, m_roomDepth * 2);
			m_generated_assetVector.push_back(asset);
			break;
		case(1):
			asset = DBG_NEW BaseActor();
			asset->setTextureTileMult(100, 10);

			asset->Init(*m_worldPtr, e_staticBody, 1, m_height / 2, m_roomDepth);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(-m_roomWidth, m_height / 2, 0);
			asset->setScale(1, m_height, m_roomDepth * 2);
			m_generated_assetVector.push_back(asset);
			
			break;
		case(2):
			asset = DBG_NEW BaseActor();
			asset->setTextureTileMult(100, 10);
			asset->Init(*m_worldPtr, e_staticBody, m_roomWidth, m_height / 2, 1);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(0, m_height / 2, m_roomDepth);
			asset->setScale(m_roomWidth * 2, m_height, 1);
			m_generated_assetVector.push_back(asset);
			break;
		case(3):
			asset = DBG_NEW BaseActor();
			asset->setTextureTileMult(100, 10);
			asset->Init(*m_worldPtr, e_staticBody, m_roomWidth, m_height / 2, 1);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(0, m_height / 2, -m_roomDepth);
			asset->setScale(m_roomWidth * 2, m_height, 1);
			m_generated_assetVector.push_back(asset);
			break;
		default:
			break;
		}
	}
}

void RoomGenerator::_placeProps()
{
	
	int toBeARandomNumberInTheFuture = 1;

	ImporterLibrary::CustomFileLoader loader;


	Manager::g_meshManager.loadStaticMesh("MOD2");
	Manager::g_meshManager.loadStaticMesh("MOD3");
	returnableRoom->CollisionBoxes = DBG_NEW BaseActor();
	ImporterLibrary::CollisionBoxes  modCollisionBoxes;
	modCollisionBoxes.boxes = nullptr;
	ImporterLibrary::PointLights lights = loader.readLightFile("MOD2");
	for (int i = 0; i < lights.nrOf; i++)
	{
		_generateLights(lights.lights[i].translate[0], lights.lights[i].translate[1], lights.lights[i].translate[2], lights.lights[i].color[0],
			lights.lights[i].color[1], lights.lights[i].color[2], lights.lights[i].intensity);
	}
	
	using namespace DirectX;
	
	int iskip = 0, jskip = 0;
	int is = rand() % (m_roomGridWidth - 1), js = rand() % (m_roomGridDepth - 1);
	is = 1;
	js = 1;
	
	int ix = -1;
	int r = rand () % 2;

	//LÄCKER MINNE MELLAN
	for (float i = -m_roomWidth + 7.5f; i <= m_roomWidth - 7.5f; i += 15.f )
	{
		int jy = -1;
		ix++;
		for (float j = -m_roomDepth + 10.f; j <= m_roomDepth - 10.f; j += 20.f )
		{
			jy++;	
			r = 2;
			if (ix == is && jy == js)
			{
				r = 3;
			
				iskip = 0;
				jskip = 1;
				if (rand() % 2)
				{
					iskip = 1;
					jskip = 0;
				}

			}
			if (ix == is + iskip && jy == js + jskip)
			{
				iskip = 0;
				jskip = 0;
				is = rand() % (ix - m_roomGridWidth - 1);
				js = rand() % (jy - m_roomGridDepth - 1);
				r = rand() % 2;
				std::cout << "I AM RANDOM" << std::endl;
				continue;
			}

			
			modCollisionBoxes = loader.readMeshCollisionBoxes("../Assets/MOD" + std::to_string(r) +"FOLDER/MOD" + std::to_string(r) +"_BBOX.bin");

			asset = DBG_NEW BaseActor();

			if (r == 3)
			{
				if (iskip == 1)
				{
					asset->setRotation(0, 90, 0, false);
					asset->setScale((20.f / 15.f), 1, 0.75f );
				}
			}
			

			XMMATRIX roomSpace = DirectX::XMLoadFloat4x4A(&asset->getWorldmatrix());
			roomSpace = DirectX::XMMatrixTranspose(roomSpace);

			//COLLISION BOXES
			for (int k = 0; k < modCollisionBoxes.nrOfBoxes && true; k++)
			{
				XMFLOAT3 cPos = XMFLOAT3(modCollisionBoxes.boxes[k].translation);
				XMFLOAT3 cScl = XMFLOAT3(modCollisionBoxes.boxes[k].scale);
				XMFLOAT4 cQuat = XMFLOAT4(modCollisionBoxes.boxes[k].rotation);

				DirectX::XMMATRIX boxTranslationMatrix = DirectX::XMMatrixTranslation(cPos.x, cPos.y, cPos.z);
				DirectX::XMMATRIX boxRotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&cQuat));
				DirectX::XMMATRIX boxScaleMatrix = DirectX::XMMatrixScaling(cScl.x, cScl.y, cScl.z);

				DirectX::XMMATRIX itemWorldSpace = boxScaleMatrix * boxRotationMatrix * boxTranslationMatrix;

				
				itemWorldSpace = itemWorldSpace * roomSpace;
				XMVECTOR decomposeTranslation;
				XMVECTOR decomposeRotation;
				XMVECTOR decomposeScaling;
			
				XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);

				modCollisionBoxes.boxes[k].translation[0] = XMVectorGetX(decomposeTranslation);
				modCollisionBoxes.boxes[k].translation[1] = XMVectorGetY(decomposeTranslation);
				modCollisionBoxes.boxes[k].translation[2] = XMVectorGetZ(decomposeTranslation);
		
				modCollisionBoxes.boxes[k].rotation[0] = XMVectorGetX(decomposeRotation);
				modCollisionBoxes.boxes[k].rotation[1] = XMVectorGetY(decomposeRotation);
				modCollisionBoxes.boxes[k].rotation[2] = XMVectorGetZ(decomposeRotation);
				modCollisionBoxes.boxes[k].rotation[3] = XMVectorGetW(decomposeRotation);
			
				modCollisionBoxes.boxes[k].scale[0] = XMVectorGetX(decomposeScaling);
				modCollisionBoxes.boxes[k].scale[1] = XMVectorGetY(decomposeScaling);
				modCollisionBoxes.boxes[k].scale[2] = XMVectorGetZ(decomposeScaling);

			}
			
			asset->Init(*m_worldPtr, modCollisionBoxes);
		
			//PROPS
			/*ImporterLibrary::PropItemToEngine tempProps = loader.readPropsFile("MOD" + std::to_string(r));
			for (int i = 0; i < tempProps.nrOfItems; i++)
			{
				XMFLOAT3 propPos = XMFLOAT3(tempProps.props[i].transform_position);
				XMFLOAT3 propScale = XMFLOAT3(tempProps.props[i].transform_position);
				//XMFLOAT4 propRotation = XMFLOAT4(tempProps.props[i].transform_rotation);
				
				DirectX::XMMATRIX propTranslationMatrix = DirectX::XMMatrixTranslation(propPos.x, propPos.y, propPos.z);
				DirectX::XMMATRIX propRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(tempProps.props[i].transform_rotation[0], tempProps.props[i].transform_rotation[1], tempProps.props[i].transform_rotation[2]);
				DirectX::XMMATRIX propScaleMatrix = DirectX::XMMatrixScaling(propScale.x, propScale.y, propScale.z);

				DirectX::XMMATRIX itemWorldSpace = propScaleMatrix * propRotationMatrix * propTranslationMatrix;


				itemWorldSpace = itemWorldSpace * roomSpace;
				XMVECTOR decomposeTranslation;
				XMVECTOR decomposeRotation;
				XMVECTOR decomposeScaling;

				XMMatrixDecompose(&decomposeScaling, &decomposeRotation, &decomposeTranslation, itemWorldSpace);

				tempProps.props[i].transform_position[0] = XMVectorGetX(decomposeTranslation);
				tempProps.props[i].transform_position[1] = XMVectorGetY(decomposeTranslation);
				tempProps.props[i].transform_position[2] = XMVectorGetZ(decomposeTranslation);

				
				tempProps.props[i].transform_rotation[0] = XMVectorGetX(decomposeRotation);
				tempProps.props[i].transform_rotation[1] = XMVectorGetY(decomposeRotation);
				tempProps.props[i].transform_rotation[2] = XMVectorGetZ(decomposeRotation);
				//tempProps.props[i].rotation[3] = XMVectorGetW(decomposeRotation);
			
				tempProps.props[i].scale[0] = XMVectorGetX(decomposeScaling);
				tempProps.props[i].scale[1] = XMVectorGetY(decomposeScaling);
				tempProps.props[i].scale[2] = XMVectorGetZ(decomposeScaling);

			}*/

			asset->Init(*m_worldPtr, modCollisionBoxes);

			//delete tempProps.props;

			if (r == 3)
			{
				asset->setPosition(i + (7.5f * iskip), 2.5, j + (10.0f * jskip), true);
			}
			else
			{
				asset->setPosition(i, 2.5, j);
				asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(7.5f, 2.5f, 10.0f));
			}
			asset->setModel(Manager::g_meshManager.getStaticMesh("MOD" + std::to_string(r)));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));
			asset->setTextureTileMult(5, 5);
			m_generated_assetVector.push_back(asset);
		
			if (modCollisionBoxes.boxes)
				delete[] modCollisionBoxes.boxes;
			if (lights.lights)
				delete[] lights.lights;
		}
	}
	//DETTA

}

void RoomGenerator::_createEnemies()
{
	Enemy * enemy;
	
	Manager::g_meshManager.loadDynamicMesh("STATE");
	Manager::g_textureManager.loadTextures("SPHERE");
	for (int i = 0; i < m_nrOfEnemies; i++)
	{
		int x = returnRandomInGridWidth();
		int z = returnRandomInGridDepth();
		enemy = new Enemy(m_worldPtr, x, 15 , z);
		DirectX::XMFLOAT4A pos = enemy->getPosition();
		Tile finPos = Tile(int(returnRandomInGridWidth()), int(returnRandomInGridDepth()));
		Tile enemyPos = m_generatedGrid->WorldPosToTile(pos.x, pos.z);
		enemy->SetPathVector(this->m_generatedGrid->FindPath(enemyPos, finPos));
		m_generatedRoomEnemies.push_back(enemy);
	}

}

void RoomGenerator::_FindWinnableAndGuardPaths()
{
	int playerStartPos = returnRandomInGridWidth();
	int playerWinsAt = returnRandomInGridWidth();

	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(playerStartPos, 6, 1 - m_roomDepth, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(playerStartPos + 2, 6, 1 - m_roomDepth, 1));


	Tile currentTile;
	Tile pathToTile;
	std::vector<Node*> nodes;
	currentTile = m_generatedGrid->WorldPosToTile(playerStartPos, 1 - m_roomDepth);
	for (int i = 0; i < 3; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			if (y == 1 && i == 6)
				pathToTile = m_generatedGrid->WorldPosToTile(playerWinsAt, m_roomDepth);
			else
				pathToTile = m_generatedGrid->WorldPosToTile(returnRandomInGridWidth(), returnRandomInGridDepth());

			std::vector<Node*> tempPath = m_generatedGrid->FindPath(currentTile, pathToTile);

			nodes.insert(std::end(nodes), std::begin(tempPath), std::end(tempPath));
			currentTile = pathToTile;
		}
	}
	currentTile = m_generatedGrid->WorldPosToTile(playerStartPos, 1 - m_roomDepth);
	for (int i = 0; i < 3; i++)
	{
		for (int y = 0; y < 3; y++)
		{

			if (y == 1 && i == 6)
				pathToTile = m_generatedGrid->WorldPosToTile(playerWinsAt, m_roomDepth);
			else
				pathToTile = m_generatedGrid->WorldPosToTile(returnRandomInGridWidth(), returnRandomInGridDepth());

			std::vector<Node*> tempPath = m_generatedGrid->FindPath(currentTile, pathToTile);

			nodes.insert(std::end(nodes), std::begin(tempPath), std::end(tempPath));
			currentTile = pathToTile;

		}
	}
	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->tile.setPathable(false);


	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->tile.setPathable(false);
		delete nodes.at(i);
	}
}

void RoomGenerator::_generateLights(float xPos, float yPos, float zPos, float colorR, float colorG, float colorB, float intensity)
{
	float lightPos[3];
	lightPos[0] = xPos;
	lightPos[1] = yPos;
	lightPos[2] = zPos;
	float color[3];
	color[0] = colorR;
	color[1] = colorG;
	color[2] = colorB;
	std::cout << lightPos[0] << " " << lightPos[1] << lightPos[2] << std::endl;
	PointLight * tempLight = DBG_NEW PointLight(lightPos, color, intensity);
	m_generated_pointLightVector.push_back(tempLight);
}

int RoomGenerator::returnRandomInGridWidth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomWidth;
	float max = m_roomWidth - 1;
	//return (rand() % (int)m_roomWidth +1) - m_roomWidth;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomInGridDepth()
{
	float randomNr = (float)rand() / RAND_MAX;
	float min = 1 - m_roomDepth;
	float max = m_roomDepth - 1;
	//return (rand() % (int)m_roomDepth +1) - m_roomDepth;
	return (min + randomNr * (max - (min)));
}

int RoomGenerator::returnRandomBetween(int min, int max)
{
	float randomNr = (float)rand() / RAND_MAX;
	return (min + randomNr * (max - (min)));
}



Room * RoomGenerator::getGeneratedRoom( b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	
	this->m_worldPtr = worldPtr;
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");
	Manager::g_meshManager.loadStaticMesh("WALL");
	Manager::g_textureManager.loadTextures("WALL");

	



	returnableRoom = new Room(worldPtr, arrayIndex, playerPtr);
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	//std::vector<Enemy*> enemies, Player * player, Grid * grid
	_generateGrid();
	//_FindWinnableAndGuardPaths();
	returnableRoom->setGrid(this->m_generatedGrid);
	_placeProps();
	

	

	_makeFloor();
	_makeWalls();
	//_createEnemies();
	
	for (int i = 0; i < 12; i++)
	{
		_generateLights(i*5, 4, i*5, 50, 60, 70, 10);	
	}
	m_generatedRoomEnemyHandler = DBG_NEW EnemyHandler;
	m_generatedRoomEnemyHandler->Init(m_generatedRoomEnemies, playerPtr, this->m_generatedGrid);

	returnableRoom->setEnemyhandler(m_generatedRoomEnemyHandler);
	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	returnableRoom->setRoomGuards(m_generatedRoomEnemies);
	return returnableRoom;
}
