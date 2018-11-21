#pragma once
#include "RandomRoomGrid.h"

class Room;
class Grid;
class PointLight;
class EnemyHandler;
class ParticleEmitter;
class RoomGenerator
{
private:
	Grid * m_generatedGrid;
	float m_roomWidth;
	float m_roomDepth;
	int m_roomGridWidth = 5;
	int m_roomGridDepth = 5;
	float m_height = 10;
	int m_nrOfWalls;
	int m_nrOfEnemies = 1;
	int incrementalValueX = 20;
	int incrementalValueY = 20; // add these instead of 20
	std::vector<BaseActor*> m_generated_assetVector;
	std::vector<PointLight*> m_generated_pointLightVector;
	std::vector<Enemy*> m_generatedRoomEnemies;
	std::vector<ParticleEmitter*> m_generated_Emitters;
	std::vector<FMOD::Geometry*> m_generatedAudioBoxes;

	Room * returnableRoom;
	b3World * m_worldPtr;
	BaseActor * asset;


	EnemyHandler * m_generatedRoomEnemyHandler;
	void dbgFuncSpawnAboveMap();

	void applyTransformationToBoundingBox(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes & boxesToModify);
	void _generateGrid();
	void _makeFloor();
	void _makeWalls();
	void _unblockIndex(RandomRoomGrid & randomizer, ImporterLibrary::GridStruct * tempGridStruct, int roomIndex);
	//void _placeProps();
	void _createEnemies(Player * playerPtr);
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

