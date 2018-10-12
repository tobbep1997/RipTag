#include "LevelHandler.h"

LevelHandler::LevelHandler()
{
}

LevelHandler::~LevelHandler()
{
	for (auto room : m_rooms)
	{
		delete room;
	}
}

void LevelHandler::Update()
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Update();
	}
}

void LevelHandler::Draw()
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Draw();
	}
}

void LevelHandler::_LoadRoom(const int roomIndex)
{
	Room * room = new Room(roomIndex);

	//Insert assets
	room->LoadRoomFromFile("");


}

void LevelHandler::UnloadRoom(const int roomNumber)
{
	m_rooms.emplace_back(roomNumber);
	m_rooms.pop_back();
}
