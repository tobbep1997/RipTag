#pragma once

class Room
{
private:
	int m_roomIndex;

public:
	Room(const short unsigned int roomIndex);
	~Room();

	void Update();

	void Draw();

	void setRoomIndex(const short unsigned int roomIndex);
	short unsigned int getRoomIndex();

private:
};
