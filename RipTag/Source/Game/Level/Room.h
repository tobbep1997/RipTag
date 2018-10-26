#pragma once
#include <string>
#include <vector>
#include "../Actors/BaseActor.h"
#include "../Actors/Assets/StaticAsset.h"
#include "../../../New_Library/ImportLibrary/FormatHeader.h"
#include "../../../New_Library/ImportLibrary/formatImporter.h"
#include "../Actors/Enemy/Enemy.h"
#include "../Actors/Player.h"
class Room
{
private:
	//RoomIndex is needed to identify what room we are in
	short unsigned int m_arrayIndex;
	
	short unsigned int m_roomIndex;
	//The assetFilPath is file we load the assets from
	//This is how we create the room 
	std::string m_assetFilePath;
	//---------------------------------------------------

	bool m_roomLoaded = false;
	std::vector<StaticAsset*> m_staticAssets;
	
	std::vector<PointLight*> m_pointLights;
	float m_playerStartPos;



	DirectX::XMFLOAT4 m_player1StartPos;
	DirectX::XMFLOAT4 m_player2StartPos;

	BaseActor * CollisionBoxes;
	Player * m_playerInRoomPtr;
	std::vector<Enemy*> m_roomGuards;
	//-------------------------------------
	//Physics
	b3World * m_worldPtr;

	std::vector<const int*> vis;
public:
	Room(const short unsigned int roomIndex, b3World * worldPtr);
	Room(const short unsigned int roomIndex, b3World * worldPtr, int arrayIndex, Player * playerPtr);
	~Room();

	void Update(float deltaTime);

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

	//---------------------------------------------------
	//Memory Management
	void UnloadRoomFromMemory();
	void LoadRoomToMemory();
private:

	
};
