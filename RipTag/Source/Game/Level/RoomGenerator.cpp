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
	m_roomDepth = rand() % (70 - 25 + 1) + 25;
	m_roomWidth = rand() % (70 - 25 + 1) + 25;
	m_generatedGrid = new Grid(-m_roomWidth, -m_roomDepth, m_roomWidth * 2, m_roomDepth * 2);
	
}

void RoomGenerator::_makeFloor()
{
	asset = new BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, m_roomWidth, 0.5, m_roomDepth);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(abs(m_roomDepth), abs(m_roomWidth));
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
	
	Manager::g_meshManager.loadStaticMesh("WAREHOUSE");
	Manager::g_textureManager.loadTextures("WAREHOUSE");
	//1 x 2 x 3
	for (int a = -m_roomDepth +1; a < m_roomDepth -1; a+= 3)
	{
		int lol = returnRandomInGridDepth();
		int lol2 = returnRandomInGridDepth();
		for (int y = 1; y < 5; y += 2)
		{
			for (int i = lol - 18; i < lol + 18; i += 3)
			{
				asset = new BaseActor();
				asset->Init(*m_worldPtr, e_staticBody, 0.5, y, 1.5);

				asset->setModel(Manager::g_meshManager.getStaticMesh("WAREHOUSE"));
				asset->setTexture(Manager::g_textureManager.getTexture("WAREHOUSE"));

				asset->setPosition(a, y, i);

				//asset->setScale(m_roomWidth * 2, m_height, 1);
				m_generated_assetVector.push_back(asset);
			}
		}
	}
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

	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(playerStartPos, 6, -m_roomDepth + 1, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(playerStartPos + 2, 6, -m_roomDepth + 1, 1));


	Tile currentTile;
	Tile pathToTile; 
	std::vector<Node*> nodes;
	currentTile = m_generatedGrid->WorldPosToTile(playerStartPos, -m_roomDepth + 1);
	for (int i = 0; i < 10; i++)
	{
		for (int y = 0; y < 10; y++)
		{

			if(y == 9 && i == 9)
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
	
	int startLol = -m_roomWidth;
	int otherStartLol = -m_roomDepth;

	for (size_t i = 0; i < m_roomWidth; i++)
	{
		startLol = -m_roomWidth;
		for (size_t j = 0; j < m_roomDepth; j++)
		{
			if (m_generatedGrid->WorldPosToTile(i, j).getPathable() == true)
				continue;
			else
			{
				asset = new BaseActor();
				asset->Init(*m_worldPtr, e_staticBody, 1, 1, 1);
				asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
				asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
				asset->setPosition(startLol, 2,	otherStartLol);

				m_generated_assetVector.push_back(asset);
			}
			startLol += 1;
		}
		otherStartLol += 1;
	}


	for (int i = 0; i < nodes.size(); i++)
		nodes[i]->tile.setPathable(false);
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
	return (rand() % (int)m_roomWidth +1) - m_roomWidth;
	return (-m_roomWidth + 1 + randomNr * (m_roomWidth - 1 -(-m_roomWidth + 1)));
}

int RoomGenerator::returnRandomInGridDepth()
{
	float randomNr = (float)rand() / RAND_MAX;
	return (rand() % (int)m_roomDepth +1) - m_roomDepth;
	return (-m_roomDepth +1 + randomNr * (m_roomDepth - 1 - (-m_roomDepth + 1)));
}

int RoomGenerator::returnRandomBetween(int min, int max)
{
	float randomNr = (float)rand() / RAND_MAX;
	return (-min + randomNr * (max - (-max)));
}



Room * RoomGenerator::getGeneratedRoom( b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	
	this->m_worldPtr = worldPtr;
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");
	Manager::g_meshManager.loadStaticMesh("WALL");
	Manager::g_textureManager.loadTextures("WALL");
	// TEMPKUB FÖR TESTANDE
	asset = new BaseActor();
	asset->Init(*m_worldPtr, e_staticBody, 1, 1.f, 1);
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(m_roomDepth, m_roomWidth);
	asset->setPosition(0, 2, 0);
	
	asset->setScale(2, 1, 2);
	m_generated_assetVector.push_back(asset);
	



	
	//std::vector<Enemy*> enemies, Player * player, Grid * grid
	_placeProps();
	returnableRoom = new Room(worldPtr, arrayIndex, playerPtr);
	_generateGrid();
	_FindWinnableAndGuardPaths();
	returnableRoom->setGrid(this->m_generatedGrid);
	
	

	_makeFloor();
	_makeWalls();
//	_createEnemies();
	_generateLights(0, 2, 0, 50, 60, 70, 10);	
	m_generatedRoomEnemyHandler = new EnemyHandler;
	m_generatedRoomEnemyHandler->Init(m_generatedRoomEnemies, playerPtr, this->m_generatedGrid);

	returnableRoom->setEnemyhandler(m_generatedRoomEnemyHandler);
	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	returnableRoom->setRoomGuards(m_generatedRoomEnemies);
	return returnableRoom;
}
