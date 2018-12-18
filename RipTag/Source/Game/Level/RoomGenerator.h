#pragma once
#include "RandomRoomGrid.h"

class Room;
class Grid;
class PointLight;
class EnemyHandler;
class ParticleEmitter;
class Torch;
class RoomGenerator
{
private:
	Grid * m_generatedGrid;
	float m_roomWidth;
	float m_roomDepth;
	int m_roomGridWidth;
	int m_roomGridDepth;
	float m_height = 10;
	int m_nrOfWalls;
	int m_nrOfEnemies = 1;
	int m_nrOfLevers = 0;
	int m_incrementalValueX = 20;
	int m_incrementalValueY = 20;

	int m_spakCounter = 0;

	std::vector<BaseActor*> m_generated_assetVector;
	std::vector<PointLight*> m_generated_pointLightVector;
	std::vector<Enemy*> m_generatedRoomEnemies;
	std::vector<ParticleEmitter*> m_generated_Emitters;
	std::vector<FMOD::Geometry*> m_generatedAudioBoxes;
	std::vector<Torch*> m_generatedTorches;
	std::vector<DirectX::BoundingBox*> m_generated_boundingBoxes;

	Room * returnableRoom;
	b3World * m_worldPtr;
	BaseActor * asset;


	EnemyHandler * m_generatedRoomEnemyHandler;
	void dbgFuncSpawnAboveMap();

	void applyTransformationToBoundingBox(DirectX::XMMATRIX roomMatrix, ImporterLibrary::CollisionBoxes & boxesToModify);
	void _generateGrid();
	void _makeFloor();
	void _makeRoof();
	void _createEntireWorld();
	void _generateGuardPaths();

	int returnRandomInGridWidth();
	int returnRandomInGridDepth();
	int returnRandomBetween(int min, int max);

	void _modifyPropBoundingBoxes(ImporterLibrary::PropItem prop);

	void moveCheckBoxes(DirectX::XMFLOAT3 startPos, ImporterLibrary::CollisionBoxes &modCollisionBoxes);

public:
	RoomGenerator();
	~RoomGenerator();
	Room * getGeneratedRoom(b3World * worldPtr, int arrayIndex, Player *  playerPtr);

};

