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

	_LoadPreFabs();
	_GenerateLevelStruct(1);

	//_LoadRoom(1);
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
	room->LoadRoomToMemory("");

	m_rooms.push_back(room);
}

void LevelHandler::_LoadPreFabs()
{
	std::experimental::filesystem::path Path = std::experimental::filesystem::current_path() / m_roomString;
	for (auto & p : std::experimental::filesystem::directory_iterator(Path))
	{
		std::wstring temp(p.path().c_str());


		m_prefabRoomFiles.push_back(std::string(temp.begin(), temp.end()));
	}

	/*for (size_t i = 0; i < m_prefabRoomFiles.size(); i++)
	{
		std::cout << m_prefabRoomFiles.at(i) << std::endl;
	}*/
}

void LevelHandler::_GenerateLevelStruct(const int seed, const int amountOfRooms)
{
	srand(seed);
	//std::vector<int> usedRooms;
	for (short unsigned int i = 0; i < amountOfRooms; i++)
	{
		//Create a room
		Room * room = new Room(i, m_worldPtr);
		//Get a random int
		int randomRoom = rand() % m_prefabRoomFiles.size();
		//Set the File path for loading and unloading
		room->setAssetFilePath(m_prefabRoomFiles.at(randomRoom));


		m_rooms.push_back(room);
	}
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
