#pragma once
#include <string>
class Room
{
private:
	short unsigned int m_roomIndex;

	

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
