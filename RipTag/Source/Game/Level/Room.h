#pragma once
#include <string>
#include <vector>
#include "../Actors/Assets/StaticAsset.h"

class Room
{
private:
	//RoomIndex is needed to identify what room we are in
	short unsigned int m_roomIndex;

	//The assetFilPath is file we load the assets from
	//This is how we create the room 
	std::string m_assetFilePath;
	//---------------------------------------------------

	bool m_roomLoaded = false;
	std::vector<StaticAsset*> m_staticAssets;

	//-------------------------------------
	//Physics
	b3World * m_worldPtr;
public:
	Room(const short unsigned int roomIndex, b3World * worldPtr);
	~Room();

	void Update();

	void Draw();

	void Release();

	void setRoomIndex(const short unsigned int roomIndex);
	short unsigned int getRoomIndex();

	void setAssetFilePath(const std::string & fileName);
	std::string getAssetFilePath();

	const bool getRoomLoaded();

	//---------------------------------------------------
	//Memory Management
	void UnloadRoomFromMemory();
	void LoadRoomToMemory();
private:

	
};
