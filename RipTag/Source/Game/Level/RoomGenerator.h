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
	
	std::vector<DirectX::BoundingBox*> m_generated_boundingBoxes;

	Room * returnableRoom;
	b3World * m_worldPtr;
	BaseActor * asset;


	EnemyHandler * m_generatedRoomEnemyHandler;
	void dbgFuncSpawnAboveMap();

	void createGridCheckBoxes(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes colBoxes, DirectX::XMFLOAT3 newPos);

	void applyTransformationToBoundingBox(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes & boxesToModify);
	void _generateGrid();
	void _makeFloor();
	void _makeWalls();
	void _createEnemies(Player * playerPtr);
	void _generateGuardPaths();

	int returnRandomInGridWidth();
	int returnRandomInGridDepth();
	int returnRandomBetween(int min, int max);

	void moveCheckBoxes(DirectX::XMFLOAT3 startPos, ImporterLibrary::CollisionBoxes &modCollisionBoxes);
	
public:
	RoomGenerator();
	~RoomGenerator();
	Room * getGeneratedRoom(b3World * worldPtr, int arrayIndex, Player *  playerPtr);

};

