#pragma once
#include <string>
#include <vector>
#include "../Actors/Assets/StaticAsset.h"

class Room
{
private:
	short unsigned int m_roomIndex;

	std::vector<StaticAsset> m_staticAssets;

public:
	Room(const short unsigned int roomIndex);
	~Room();

	void Update();

	void Draw();

	void setRoomIndex(const short unsigned int roomIndex);
	short unsigned int getRoomIndex();

	void LoadRoomFromFile(const std::string & fileName);
private:
};
