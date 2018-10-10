#include "Room.h"

Room::Room(const short unsigned int roomIndex)
{
	m_roomIndex = roomIndex;
}

Room::~Room()
{
}

void Room::setRoomIndex(const short unsigned int roomIndex)
{
	this->m_roomIndex = roomIndex;
}

unsigned short int Room::getRoomIndex()
{
	return this->m_roomIndex;
}

void Room::Update()
{
}

void Room::Draw()
{
}
