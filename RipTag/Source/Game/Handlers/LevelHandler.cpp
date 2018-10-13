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

void LevelHandler::Init(b3World& worldPtr)
{
	m_worldPtr = &worldPtr;

	_GenerateLevelStruct(1);

	_LoadRoom(1);
	//testtt.Init(m_worldPtr, );
}

void LevelHandler::Release()
{
	for (int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Release();
	}
}

void LevelHandler::Update(float deltaTime)
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Update();
	}

	//testCube->Update(deltaTime);
}

void LevelHandler::Draw()
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Draw();
	}
	//testCube->Draw();
}

void LevelHandler::_LoadRoom(const int roomIndex)
{
	Room * room = new Room(roomIndex, m_worldPtr);

	//Insert assets
	room->LoadRoomFromFile("");

	m_rooms.push_back(room);
}

void LevelHandler::_GenerateLevelStruct(const int seed)
{
	std::experimental::filesystem::path Path = std::experimental::filesystem::current_path() / "../Assets/ROOMSPREFAB";
	for (auto & p : std::experimental::filesystem::directory_iterator(Path))
		std::cout << p << std::endl;
	//std::cout << std::experimental::filesystem::current_path() << std::endl;
}

void LevelHandler::UnloadRoom(const int roomNumber)
{
	for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		if (m_rooms.at(i)->getRoomIndex() == roomNumber)
		{
			m_rooms.erase(m_rooms.begin() + i);
		}
	}
}
