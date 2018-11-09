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
	m_roomDepth = rand() % (35 - 25 + 1) + 25;
	m_roomWidth = rand() % (35 - 25 + 1) + 25;
	m_generatedGrid = new Grid(-m_roomDepth, -m_roomWidth, m_roomWidth * 2, m_roomDepth * 2);
	
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
	return (-m_roomWidth + randomNr * (m_roomWidth -(-m_roomWidth)));
}

int RoomGenerator::returnRandomInGridDepth()
{
	float randomNr = (float)rand() / RAND_MAX;
	return (-m_roomDepth + randomNr * (m_roomDepth - (-m_roomDepth)));
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
	



	Room * returnableRoom;
	_generateGrid();
	//std::vector<Enemy*> enemies, Player * player, Grid * grid
	_placeProps();
	returnableRoom = new Room(worldPtr, arrayIndex, playerPtr);
	returnableRoom->setGrid(this->m_generatedGrid);
	
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 6, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 6, 0, 1));

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
