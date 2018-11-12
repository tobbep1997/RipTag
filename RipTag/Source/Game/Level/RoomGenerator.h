#pragma once

class Room;
class Grid;
class PointLight;
class EnemyHandler;
class RoomGenerator
{
private:
	Grid * m_generatedGrid;
	float m_roomWidth;
	float m_roomDepth;
	float m_height = 10;
	int m_nrOfWalls;
	int m_nrOfEnemies = 5;
	std::vector<BaseActor*> m_generated_assetVector;
	std::vector<PointLight*> m_generated_pointLightVector;
	std::vector<Enemy*> m_generatedRoomEnemies;
	Room * returnableRoom;
	b3World * m_worldPtr;
	BaseActor * asset;

	EnemyHandler * m_generatedRoomEnemyHandler;


	void _generateGrid();
	void _makeFloor();
	void _makeWalls();
	void _placeProps();
	void _createEnemies();
	void _FindWinnableAndGuardPaths();
	void _generateLights(float xPos, float yPos, float zPos, float colorR, float colorG, float colorB, float intensity);

	int returnRandomInGridWidth();
	int returnRandomInGridDepth();
	int returnRandomBetween(int min, int max);
public:
	RoomGenerator();
	~RoomGenerator();
	Room * getGeneratedRoom(b3World * worldPtr, int arrayIndex, Player *  playerPtr);

};

