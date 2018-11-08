#pragma once
#include <string>
#include <vector>
#include <AudioEngine.h>
#include "2D Engine/Quad/Components/HUDComponent.h"

namespace ImporterLibrary {
	struct GridStruct;
	struct PropItemToEngine;
}

class Quad;
class Grid;
class Door;
class EnemyHandler;
class Lever;
class Player;
class BaseActor;
class PointLight;
class StaticAsset;
class PressurePlate;
class TriggerHandler;
class Triggerable;
class Trigger;
class Bars;

class Enemy;//Ta bort sen

class Room : public HUDComponent
{
private:
	
private:
	//RoomIndex is needed to identify what room we are in
	short unsigned int m_arrayIndex;
	
	short unsigned int m_roomIndex;
	//The assetFilPath is file we load the assets from
	//This is how we create the room 
	std::string m_assetFilePath;
	//---------------------------------------------------

	bool m_roomLoaded = false;
	std::vector<StaticAsset*> m_staticAssets;	//Released
	std::vector<PointLight*> m_pointLights;		//Released
	std::vector<FMOD::Geometry*> m_audioBoxes;	//Released
	float m_playerStartPos;

	ImporterLibrary::GridStruct * m_grid;
	Grid * m_pathfindingGrid;
	
	DirectX::XMFLOAT4 m_player1StartPos;
	DirectX::XMFLOAT4 m_player2StartPos;

	BaseActor * CollisionBoxes;
	Player * m_playerInRoomPtr;
	std::vector<Enemy*> m_roomGuards;
	//-------------------------------------
	//Physics
	b3World * m_worldPtr;

	TriggerHandler * triggerHandler;


	
	//std::vector<StaticAsset*> TODO:: FIX

	void placeRoomProps(ImporterLibrary::PropItemToEngine propsToPlace);

	std::vector<const int*> vis;

	Quad * m_lose;

	bool m_youLost = false;
public:
	Room(const short unsigned int roomIndex, b3World * worldPtr);
	Room(const short unsigned int roomIndex, b3World * worldPtr, int arrayIndex, Player * playerPtr);
	~Room();

	void Update(float deltaTime);

	void SetActive(bool state);

	void Draw();

	void Release();

	void loadTextures();

	void setRoomIndex(const short unsigned int roomIndex);
	short unsigned int getRoomIndex();

	void setAssetFilePath(const std::string & fileName);
	std::string getAssetFilePath();

	const bool getRoomLoaded();

	BaseActor * getCollissionBox() { return CollisionBoxes; };

	DirectX::XMFLOAT4 getPlayer1StartPos();
	DirectX::XMFLOAT4 getPlayer2StartPos();

	const std::vector<Enemy*>* getEnemies() const;

	//---------------------------------------------------
	//Memory Management
	void UnloadRoomFromMemory();
	void LoadRoomToMemory();

	// Test section
	void getPath();
private:

	
};
