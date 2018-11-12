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
	std::vector<StaticAsset*> m_generated_assetVector;
	std::vector<PointLight*> m_generated_pointLightVector;
	b3World * m_worldPtr;
	StaticAsset * asset;

	void _generateGrid();
	void _makeFloor();
	void _generateLights(float xPos, float yPos, float zPos, float colorR, float colorG, float colorB, float intensity);

public:
	RoomGenerator();
	~RoomGenerator();
	Room * getGeneratedRoom(b3World * worldPtr, int arrayIndex, Player *  playerPtr);

};

