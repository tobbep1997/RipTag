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
	m_roomDepth = 45+2;
	m_roomWidth = 40+2;
	m_generatedGrid = new Grid(-m_roomWidth, -m_roomDepth, m_roomWidth * 2, m_roomDepth * 2);
	
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

	
	for (int i = 0; i < m_nrOfWalls; i++)
	{
		switch (i)
		{
		case(0):
			asset = new BaseActor();
			asset->Init(*m_worldPtr, e_staticBody, 1, m_height / 2, m_roomDepth);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(m_roomWidth, m_height / 2, 0);
			asset->setScale(1, m_height, m_roomDepth * 2);
			m_generated_assetVector.push_back(asset);
			break;
		case(1):
			asset = new BaseActor();
			asset->Init(*m_worldPtr, e_staticBody, 1, m_height / 2, m_roomDepth);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(-m_roomWidth, m_height / 2, 0);
			asset->setScale(1, m_height, m_roomDepth * 2);
			m_generated_assetVector.push_back(asset);
			
			break;
		case(2):
			asset = new BaseActor();
			asset->Init(*m_worldPtr, e_staticBody, m_roomWidth, m_height / 2, 1);
			asset->setModel(Manager::g_meshManager.getStaticMesh("WALL"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));

			asset->setPosition(0, m_height / 2, m_roomDepth);
			asset->setScale(m_roomWidth * 2, m_height, 1);
			m_generated_assetVector.push_back(asset);
			break;
		case(3):
			asset = new BaseActor();
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
	returnableRoom->CollisionBoxes = new BaseActor();
	ImporterLibrary::CollisionBoxes  modCollisionBoxes;
	modCollisionBoxes = loader.readMeshCollisionBoxes("../Assets/MOD2FOLDER/MOD2_BBOX.bin");
	
	ImporterLibrary::PointLights lights = loader.readLightFile("MOD2");
	for (int i = 0; i < lights.nrOf; i++)
	{
		_generateLights(lights.lights[i].translate[0], lights.lights[i].translate[1], lights.lights[i].translate[2], lights.lights[i].color[0],
			lights.lights[i].color[1], lights.lights[i].color[2], lights.lights[i].intensity);
	}
	using namespace DirectX;
	for (float i = -m_roomWidth + 7.5f; i < m_roomWidth - 7.5f; i += 15.f )
	{
		for (float j = -m_roomDepth + 10.f; j < m_roomDepth - 10.f; j += 20.f )
		{
			asset = new BaseActor();
			asset->setPosition(i, 2.5, j, false);
			asset->setScale(1, 1, 1);

			XMMATRIX worldMatrix = (XMLoadFloat4x4A(&asset->getWorldmatrix()));
			XMVECTOR world;
			XMVECTOR scale;
			XMVECTOR rot;

			for (int k = 0; k < modCollisionBoxes.nrOfBoxes && true; k++)
			{
				world = XMLoadFloat3(&XMFLOAT3(modCollisionBoxes.boxes[k].translation));
				scale = XMLoadFloat3(&XMFLOAT3(modCollisionBoxes.boxes[k].scale));
				rot = XMLoadFloat4(&XMFLOAT4(modCollisionBoxes.boxes[k].rotation));
				world = XMVector3Transform(world, worldMatrix);
				scale = XMVector3Transform(scale, worldMatrix);

				modCollisionBoxes.boxes[k].translation[0] = XMVectorGetX(world);
				modCollisionBoxes.boxes[k].translation[1] = XMVectorGetY(world);
				modCollisionBoxes.boxes[k].translation[2] = XMVectorGetZ(world);

				modCollisionBoxes.boxes[k].scale[0] = XMVectorGetX(scale);
				modCollisionBoxes.boxes[k].scale[1] = XMVectorGetY(scale);
				modCollisionBoxes.boxes[k].scale[2] = XMVectorGetZ(scale);

			}

			asset->Init(*m_worldPtr, modCollisionBoxes);
			asset->setPosition(i, 2.5, j, true);
			//p_createbbox
			asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(7.5f, 2.5f, 10.0f));
			asset->setModel(Manager::g_meshManager.getStaticMesh("MOD2"));
			asset->setTexture(Manager::g_textureManager.getTexture("WALL"));
			asset->setTextureTileMult(5, 5);
			m_generated_assetVector.push_back(asset);
		}
	}

	//Manager::g_meshManager.loadStaticMesh("WAREHOUSE");
	//Manager::g_textureManager.loadTextures("WAREHOUSE");
	////1 x 2 x 3
	//for (int a = -m_roomDepth +1; a < m_roomDepth -1; a+= 3)
	//{
	//	int lol = returnRandomInGridDepth();
	//	int lol2 = returnRandomInGridDepth();
	//	for (int y = 1; y < 5; y += 2)
	//	{
	//		for (int i = lol - 18; i < lol + 18; i += 3)
	//		{
	//			asset = new BaseActor();
	//			asset->Init(*m_worldPtr, e_staticBody, 0.5, y, 1.5);

	//			asset->setModel(Manager::g_meshManager.getStaticMesh("WAREHOUSE"));
	//			asset->setTexture(Manager::g_textureManager.getTexture("WAREHOUSE"));

			//	asset->setPosition(a, y, i);
				//asset->p_createBoundingBox(DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT3(0.5 * 2, 0.5 * 2, 1.5 * 2));
				

	//			//asset->setScale(m_roomWidth * 2, m_height, 1);
	//			m_generated_assetVector.push_back(asset);
	//		}
	//	}
	//}

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
	PointLight * tempLight = new PointLight(lightPos, color, intensity);
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

	



	
	//std::vector<Enemy*> enemies, Player * player, Grid * grid
	returnableRoom = new Room(worldPtr, arrayIndex, playerPtr);
	_generateGrid();
	//_FindWinnableAndGuardPaths();
	returnableRoom->setGrid(this->m_generatedGrid);
	_placeProps();
	

	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 10, 0, 1));

	_makeFloor();
	_makeWalls();
//	_createEnemies();
	
	for (int i = 0; i < 12; i++)
	{
		_generateLights(i*5, 4, i*5, 50, 60, 70, 10);	

	}
	m_generatedRoomEnemyHandler = new EnemyHandler;
	m_generatedRoomEnemyHandler->Init(m_generatedRoomEnemies, playerPtr, this->m_generatedGrid);

	returnableRoom->setEnemyhandler(m_generatedRoomEnemyHandler);
	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	returnableRoom->setRoomGuards(m_generatedRoomEnemies);
	return returnableRoom;
}
