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

	m_roomDepth = rand() % (35 - 25 + 1) + 25;
	m_roomWidth = rand() % (35 - 25 + 1) + 25;
	m_roomDepth = -m_roomDepth;
	m_roomWidth = -m_roomWidth;
	m_generatedGrid = new Grid(m_roomDepth, m_roomWidth, m_roomWidth * 2, m_roomDepth * 2);

}

void RoomGenerator::_makeFloor()
{
	asset = new StaticAsset;
	asset->setModel(Manager::g_meshManager.getStaticMesh("FLOOR"));
	asset->setTexture(Manager::g_textureManager.getTexture("FLOOR"));
	asset->setTextureTileMult(25, 25);
	asset->setPosition(0, 0, 0);
	asset->setScale(abs(m_roomDepth*2), 1.f, abs(m_roomWidth*2));
	asset->Init(*m_worldPtr, m_roomDepth, 1.f, m_roomWidth);
	m_generated_assetVector.push_back(asset);
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

Room * RoomGenerator::getGeneratedRoom( b3World * worldPtr, int arrayIndex, Player *  playerPtr)
{
	this->m_worldPtr = worldPtr;
	Manager::g_meshManager.loadStaticMesh("FLOOR");
	Manager::g_textureManager.loadTextures("FLOOR");


	Room * returnableRoom;
	_generateGrid();

	returnableRoom = new Room(worldPtr, arrayIndex, playerPtr);
	returnableRoom->setGrid(this->m_generatedGrid);
	
	returnableRoom->setPlayer1StartPos(DirectX::XMFLOAT4(0, 5, 0, 1));
	returnableRoom->setPlayer2StartPos(DirectX::XMFLOAT4(0, 5, 0, 1));

	_makeFloor();
	_generateLights(0, 3, 0, 200, 102, 50, 10);
	
	



	returnableRoom->setStaticMeshes(m_generated_assetVector);
	returnableRoom->setLightvector(m_generated_pointLightVector);
	return returnableRoom;
}
