#pragma once

class Room;
class Grid;
class PointLight;

class RoomGenerator
{
private:
	Grid * m_generatedGrid;
	float m_roomWidth;
	float m_roomDepth;
	float m_height = 10;
	int m_nrOfWalls;
	std::vector<BaseActor*> m_generated_assetVector;
	std::vector<PointLight*> m_generated_pointLightVector;
	b3World * m_worldPtr;
	BaseActor * asset;

	void _generateGrid();
	void _makeFloor();
	void _makeWalls();
	void _generateLights(float xPos, float yPos, float zPos, float colorR, float colorG, float colorB, float intensity);

public:
	RoomGenerator();
	~RoomGenerator();
	Room * getGeneratedRoom(b3World * worldPtr, int arrayIndex, Player *  playerPtr);

};

