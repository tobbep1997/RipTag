#pragma once
#include <string>
#include <vector>
#include "../Actors/Assets/StaticAsset.h"

class Room
{
private:
	short unsigned int m_roomIndex;

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

	void LoadRoomFromFile(const std::string & fileName);
private:
};
