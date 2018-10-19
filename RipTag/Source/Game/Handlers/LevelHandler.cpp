#include "LevelHandler.h"
#include "InputManager/InputHandler.h"
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
	m_activeRoom = 0;
	m_worldPtr = &worldPtr;

	_LoadPreFabs();
	_GenerateLevelStruct(2);

	_RoomLoadingManager();

	
	//future = std::async(std::launch::async, &LevelHandler::_RoomLoadingManager, this, m_activeRoom);

	
	//_LoadRoom(1);
	//testtt.Init(m_worldPtr, );
}

void LevelHandler::Release()
{
	
	for (int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Release();
	}
	for (size_t i = 0; i < m_rooms.size(); i++)
	{
		m_rooms.at(i)->getCollissionBox();
	}
}

void LevelHandler::Update(float deltaTime)
{
	using namespace std::chrono_literals;
	if (future.valid())
	{
		auto status = future.wait_for(0s);
		if (status == std::future_status::ready) {
			std::cout << "Thread finished" << std::endl;
			future.get();
		}
		else {
			//std::cout << "Thread still running" << std::endl;
		}
	}

	m_rooms.at(m_activeRoom)->Update();
	if (InputHandler::isKeyPressed('N'))
	{
		if (pressed == false)
		{
			m_activeRoom--;
			std::cout << m_activeRoom << std::endl;
			_RoomLoadingManager();
			pressed = true;
			DirectX::XMFLOAT4 startPos = m_rooms.at(m_activeRoom)->getPlayer2StartPos();
			this->m_playerPtr->setPosition(startPos.x, startPos.y, startPos.z, startPos.w);
		}
	}
	else if (InputHandler::isKeyPressed('M'))
	{
		if (pressed == false)
		{
			m_activeRoom++;
			std::cout << m_activeRoom << std::endl;
			_RoomLoadingManager();
			pressed = true;
			DirectX::XMFLOAT4 startPos = m_rooms.at(m_activeRoom)->getPlayer2StartPos();
			this->m_playerPtr->setPosition(startPos.x, startPos.y, startPos.z, startPos.w);
		}
	}
	else
	{
		pressed = false;
	}
}

void LevelHandler::Draw()
{
	/*for (unsigned int i = 0; i < m_rooms.size(); ++i)
	{
		m_rooms.at(i)->Draw();
	}*/
	m_rooms.at(m_activeRoom)->Draw();
	//testCube->Draw();
}

void LevelHandler::setPlayer(Player * playerPtr)
{
	this->m_playerPtr = playerPtr;
}

void LevelHandler::_LoadPreFabs()
{
	
}

void LevelHandler::_GenerateLevelStruct(const int seed, const int amountOfRooms)
{
	srand(seed);
	//std::vector<int> usedRooms;
	for (short unsigned int i = 0; i < amountOfRooms; i++)
	{
		//Create a room
		//Get a random int
		int randomRoom = rand() % 2+1;
		Room * room = new Room(randomRoom, m_worldPtr, i, m_playerPtr);

		//Set the File path for loading and unloading
		//room->setAssetFilePath(m_prefabRoomFiles.at(randomRoom));


		m_rooms.push_back(room);
	}
}

void LevelHandler::_RoomLoadingManager(short int room)
{
	short int current;
	if (room == -1)
	{
		current = m_activeRoom;
	}
	else
	{
		current = room;
	}

	//Room Unload and Load
	if ((current - 2) >= 0)
	{
		//m_rooms.at(current - 2)->UnloadRoomFromMemory();

		m_unloadMutex.lock();
		m_unloadingQueue.push_back(current- 2);
		m_unloadMutex.unlock();
	}

	if ((current - 1) >= 0)
	{
		//m_rooms.at(current)->LoadRoomToMemory();

		m_loadMutex.lock();
		m_loadingQueue.push_back(current - 1);
		m_loadMutex.unlock();
	}
	

	m_rooms.at(current)->loadTextures();
	m_rooms.at(current)->LoadRoomToMemory();

	if ((current + 1) < m_rooms.size())
	{
		//m_rooms.at(current + 1)->LoadRoomToMemory();

		m_loadMutex.lock();
		m_loadingQueue.push_back(current + 1);
		m_loadMutex.unlock();
	}

	if ((current + 2) < m_rooms.size())
	{
		//m_rooms.at(current + 2)->UnloadRoomFromMemory();

		m_unloadMutex.lock();
		m_unloadingQueue.push_back(current + 2);
		m_unloadMutex.unlock();
	}
	for (unsigned int i = 0; i < m_loadingQueue.size(); i++)
		m_rooms.at(m_loadingQueue.at(i))->loadTextures();

	future = std::async(std::launch::async, &LevelHandler::_RoomLoadingThreading, this);
	
	
	//m_loadingQueue.clear();

}

void LevelHandler::_RoomLoadingThreading()
{
	for (unsigned int i = 0; i < m_loadingQueue.size(); i++)
	{
		m_rooms.at(m_loadingQueue.at(i))->LoadRoomToMemory();
	}
	m_loadingQueue.clear();
	for (unsigned int i = 0; i < m_unloadingQueue.size(); i++)
	{
		m_rooms.at(m_unloadingQueue.at(i))->UnloadRoomFromMemory();
	}
	m_unloadingQueue.clear();
}

