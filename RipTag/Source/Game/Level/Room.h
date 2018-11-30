#pragma once
#include <string>
#include <vector>
#include <AudioEngine.h>
#include "2D Engine/Quad/Components/HUDComponent.h"
#include "../Handlers/EnemyHandler.h"

namespace ImporterLibrary {
	struct GridStruct;
	struct PropItemToEngine;
	struct PropItem;
}

class Quad;
class Grid;
class Door;
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
class ParticleEmitter;
class Torch;

class Enemy;//Ta bort sen

class Room : public HUDComponent
{
private:
	std::vector<Torch*> m_Torches;
private:
	//RoomIndex is needed to identify what room we are in
	short unsigned int m_arrayIndex;
	
	short int m_roomIndex;
	//The assetFilPath is file we load the assets from
	//This is how we create the room 
	std::string m_assetFilePath;
	//---------------------------------------------------

	bool m_roomLoaded = false;
	std::vector<BaseActor*> m_staticAssets;	//Released
	std::vector<FMOD::Geometry*> m_audioBoxes;	//Released
	float m_playerStartPos;

	std::vector<PointLight*> m_pointLights;

	ImporterLibrary::GridStruct * m_grid;
	Grid * m_pathfindingGrid;
	
	DirectX::XMFLOAT4 m_player1StartPos;
	DirectX::XMFLOAT4 m_player2StartPos;

	
	Player * m_playerInRoomPtr;
	std::vector<Enemy*> m_roomGuards;
	EnemyHandler * m_enemyHandler;
	//-------------------------------------
	//Physics
	b3World * m_worldPtr;

	TriggerHandler * triggerHandler;

	
	//std::vector<StaticAsset*> TODO:: FIX

	void _createAudioBox(ImporterLibrary::PropItem prop, bool useAudio, float occlusion, float reverbOcclusion);
	void placeRoomProps(ImporterLibrary::PropItemToEngine propsToPlace);

	//std::vector<const int*> vis;
	Quad * m_lose;
	bool m_youLost = false;

public:
	BaseActor * CollisionBoxes = nullptr;
	Room(const short unsigned int roomIndex, b3World * worldPtr);
	Room(const short unsigned int roomIndex, b3World * worldPtr, int arrayIndex, Player * playerPtr);
	Room(b3World * worldPtr, int arrayIndex, Player * playerPtr);
	~Room();

	void Update(float deltaTime, Camera * camera);

	void SetActive(bool state);

	void Draw();

	void Release();

	void loadTextures();

	void setRoomIndex(const short unsigned int roomIndex);
	short int getRoomIndex();

	void setAssetFilePath(const std::string & fileName);
	std::string getAssetFilePath();

	const bool getRoomLoaded();
	EnemyHandler * getEnemyHandler() { return m_enemyHandler; }

	BaseActor * getCollissionBox() { return CollisionBoxes; }

	DirectX::XMFLOAT4 getPlayer1StartPos();
	DirectX::XMFLOAT4 getPlayer2StartPos();

	const std::vector<Enemy*>* getEnemies() const;
	void GiveCameraToParticles(Camera * ptr);

	//---------------------------------------------------
	//Memory Management
	void UnloadRoomFromMemory();
	void LoadRoomToMemory();

	//RoomGeneration
	void loadTriggerPairMap();
	void setGrid(Grid * gridToset) { this->m_pathfindingGrid = gridToset; };
	void setPlayer1StartPos(DirectX::XMFLOAT4 startPos) { this->m_player1StartPos = startPos; };
	void setPlayer2StartPos(DirectX::XMFLOAT4 startPos) { this->m_player2StartPos = startPos; };
	void setStaticMeshes(std::vector<BaseActor*> assets) { this->m_staticAssets = assets; };
	void setLightvector(std::vector<PointLight*> lights) { this->m_pointLights = lights; };
	//void setParticleEmitterVector(std::vector<ParticleEmitter*> emitter) { this->m_emitters = emitter; };
	Player * getPLayerInRoomPtr() { return this->m_playerInRoomPtr; };
	TriggerHandler * getTriggerHandler() { return triggerHandler; }
	void setEnemyhandler(EnemyHandler * enemyHandlerPtr) { this->m_enemyHandler = enemyHandlerPtr; };
	void setRoomGuards(std::vector<Enemy*> guardsPtr) { this->m_roomGuards = guardsPtr; };
	void setAudioBoxes(std::vector<FMOD::Geometry*> audioBoxes) { this->m_audioBoxes = audioBoxes; };
	void setTorches(std::vector<Torch*> torches) { this->m_Torches = torches; };
	void addPropsAndAssets(ImporterLibrary::PropItemToEngine propsAndAssets, TriggerHandler * triggerHandler, std::vector<BaseActor*> * assetVector, bool isRandomRoom = false);

	std::vector<FMOD::Geometry*>* getAudioBoxesVector() { return &this->m_audioBoxes; };

	void setLoaded(const bool & loaded) { this->m_roomLoaded = loaded; }

private:
	void _setPropAttributes(ImporterLibrary::PropItem prop, const std::string & name, std::vector<BaseActor*> * assetVector, bool useBoundingBox = false, bool isRandomRoom = false, bool useAudio = false, float occlusionSound = 0.75f, float reverbOcclusionSound = 0.35f);
	void _addToTriggerHandler(ImporterLibrary::PropItem prop, const std::string & name, TriggerHandler * triggerHandler, bool animated, int index);
};
