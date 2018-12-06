#include "RipTagPCH.h"
#include "LevelHandler.h"



LevelHandler::LevelHandler(const unsigned short & roomIndex)
{
	m_roomIndex = roomIndex;
}

LevelHandler::~LevelHandler()
{
	for (auto room : m_rooms)
	{
		delete room;
	}
}

void LevelHandler::Init(b3World& worldPtr, Player * playerPtr, const int & seed, const int & roomIndex)
{
	m_playerPtr = playerPtr;
	m_activeRoom = 0;
	m_worldPtr = &worldPtr;

	_LoadCorrectRoom(seed,roomIndex);

	_RoomLoadingManager();
	m_rooms[m_activeRoom]->SetActive(true);
	m_rooms[m_activeRoom]->GiveCameraToParticles(playerPtr->getCamera());

	this->m_playerPtr->setPosition(m_rooms.at(0)->getPlayer1StartPos().x,
		m_rooms.at(0)->getPlayer1StartPos().y,
		m_rooms.at(0)->getPlayer1StartPos().z);
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

void LevelHandler::Update(float deltaTime, Camera * camera)
{
	//Is not used - but do not remove
	/*using namespace std::chrono_literals;
	if (future.valid())
	{
		auto status = future.wait_for(0s);
		if (status == std::future_status::ready) {
			future.get();
		}
		else {
		}
	}*/

	
	if (InputHandler::isKeyPressed('N'))
	{
		if (pressed == false)
		{
			m_rooms[m_activeRoom]->SetActive(false);
			//m_activeRoom--;
			m_activeRoom = 0;
			_RoomLoadingManager();
			pressed = true;
			DirectX::XMFLOAT4 startPos = m_rooms.at(m_activeRoom)->getPlayer1StartPos();
			this->m_playerPtr->setPosition(startPos.x, startPos.y, startPos.z, startPos.w);
			m_rooms[m_activeRoom]->SetActive(true);
			RipExtern::g_rayListener->ClearRays();
			RipExtern::g_rayListener->ClearProcessedRays();
			RipExtern::g_contactListener->ClearContactQueue();
			RipExtern::g_kill = true;
		}
	}
	else if (InputHandler::isKeyPressed('M'))
	{
		
		if (pressed == false)
		{
			m_rooms[m_activeRoom]->SetActive(false);
			//m_activeRoom++;
			m_activeRoom = 0;
			_RoomLoadingManager();
			pressed = true;
			DirectX::XMFLOAT4 startPos = m_rooms.at(m_activeRoom)->getPlayer1StartPos();
			this->m_playerPtr->setPosition(startPos.x, startPos.y, startPos.z, startPos.w);
			m_rooms[m_activeRoom]->SetActive(true);

			RipExtern::g_rayListener->ClearRays();
			RipExtern::g_rayListener->ClearProcessedRays();
			RipExtern::g_contactListener->ClearContactQueue();
			RipExtern::g_kill = true;
		}
	}
	else
	{
	
		pressed = false;
	}
	if (RipExtern::g_kill == false)
	{
		m_rooms.at(m_activeRoom)->Update(deltaTime, camera);
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

const std::vector<Enemy*>* LevelHandler::getEnemies() const
{
	return m_rooms[m_activeRoom]->getEnemies();
}

TriggerHandler * LevelHandler::getTriggerHandler()
{
	return m_rooms[m_activeRoom]->getTriggerHandler();
}

EnemyHandler * LevelHandler::getEnemyHandler()
{
	return m_rooms[m_activeRoom]->getEnemyHandler();
}

std::vector<Room*> LevelHandler::getRooms()
{
	return m_rooms; 
}


std::tuple<DirectX::XMFLOAT4, DirectX::XMFLOAT4> LevelHandler::getStartingPositions()
{
	return std::tuple<DirectX::XMFLOAT4, DirectX::XMFLOAT4>(this->m_rooms[0]->getPlayer1StartPos(), this->m_rooms[0]->getPlayer2StartPos());
}

const unsigned short LevelHandler::getNextRoom() const
{
	return this->m_nextRoomIndex;
}

short unsigned int LevelHandler::getActiveRoomNr() const
{
	return m_activeRoom; 
}

bool LevelHandler::HasMoreRooms()
{
	if (m_activeRoom == 2)
		return false;
	else
		return true;
}

void LevelHandler::LoadNextRoom(int player)
{
	m_rooms[m_activeRoom]->SetActive(false);

	m_activeRoom = 0;
	
	_RoomLoadingManager();

	DirectX::XMFLOAT4 startPos; 
	if (player == 1)
		startPos = m_rooms.at(m_activeRoom)->getPlayer1StartPos();
	else
		startPos = m_rooms.at(m_activeRoom)->getPlayer2StartPos();

	this->m_playerPtr->setPosition(startPos.x, startPos.y, startPos.z, startPos.w);

	m_rooms[m_activeRoom]->SetActive(true);

	RipExtern::g_rayListener->ClearRays();
	RipExtern::g_rayListener->ClearProcessedRays();
	RipExtern::g_contactListener->ClearContactQueue();

	RipExtern::g_kill = true;
}

void LevelHandler::_LoadCorrectRoom(const int& seed, const int& roomIndex)
{
	Room * room;
	srand(seed);
	if (roomIndex != -1)
	{
		room = m_roomGenerator.getGeneratedRoom(m_worldPtr, 1, m_playerPtr);
		room->setLoaded(true);
	}
	else
	{
		room = new Room(roomIndex, m_worldPtr, 0, m_playerPtr);
	}
	m_rooms.push_back(room);

	m_rooms.at(0)->loadTextures();
	int x = m_rooms.at(0)->getRoomIndex();
	if (m_rooms.at(0)->getRoomIndex() != -1)
		m_rooms.at(0)->LoadRoomToMemory();
}

void LevelHandler::_GenerateLevelStruct(const int seed, const int amountOfRooms)
{
	//srand(seed);
	//std::vector<int> usedRooms;
	////LoadTuTorialRoomFirst
	//Room * room = new Room(m_roomIndex, m_worldPtr, 0, m_playerPtr);
	//m_rooms.push_back(room);
	//room = new Room(1, m_worldPtr, 1, m_playerPtr);
	//m_rooms.push_back(room);
	//room = new Room(0, m_worldPtr, 2, m_playerPtr);
	//m_rooms.push_back(room);
	//m_rooms.at(0)->loadTextures();
	//int x = m_rooms.at(0)->getRoomIndex();
	//if(m_rooms.at(0)->getRoomIndex() != -1)
	//m_rooms.at(0)->LoadRoomToMemory();

	//                               //Byt i < 1 till amountOfRooms
	//for (short unsigned int i = 0; i < 1; i++)
	//{
	//	//Create a room
	//	//Get a random int					//VERY NECCESSARY TO COMMENT BACK IN
	//	int randomRoom = rand() % amountOfRooms;
	//	Room * room = new Room(1, m_worldPtr, i+3, m_playerPtr);//TODO


	//	m_rooms.push_back(room);
	//}
	//add to loop -> 1 0 i;
	//Room * room = new Room(0, m_worldPtr, 0, m_playerPtr);
	//m_rooms.push_back(room);

	//Room * room = m_roomGenerator.getGeneratedRoom(m_worldPtr, 1, m_playerPtr);
	//m_rooms.push_back(room);
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
	//if ((current - 2) >= 0)
	//{
	//	//m_rooms.at(current - 2)->UnloadRoomFromMemory();

	//	m_unloadMutex.lock();
	//	m_unloadingQueue.push_back(current- 2);
	//	m_unloadMutex.unlock();
	//}
	if ((current - 1) >= 0)
	{
		//m_rooms.at(current)->LoadRoomToMemory();

		/*m_loadMutex.lock();
		m_loadingQueue.push_back(current - 1);
		m_loadMutex.unlock();*/
		/*if (m_rooms.at(current - 1)->getAssetFilePath() != "RUM3")
		{

		}*/
		const b3Body * world = RipExtern::g_world->getBodyList();
		u32 wc = RipExtern::g_world->GetBodyCount();
		//m_rooms.at(current - 1)->UnloadRoomFromMemory();
		//m_nextRoomIndex = m_roomIndex + 1; //TODO::THIS
	}
	if ((current + 1) < m_rooms.size())
	{
		//m_rooms.at(current + 1)->LoadRoomToMemory();

		//m_loadMutex.lock();
		//m_loadingQueue.push_back(current + 1);
		//m_loadMutex.unlock();
		/*if (m_rooms.at(current + 1)->getAssetFilePath() != "RUM3")
		{

		}*/
		const b3Body * world = RipExtern::g_world->getBodyList();
		u32 wc = RipExtern::g_world->GetBodyCount();

		//m_rooms.at(current + 1)->UnloadRoomFromMemory();
	}
	m_nextRoomIndex = m_roomIndex + 1;
	/*
	if (m_roomIndex == 0)
		m_nextRoomIndex = 1;
	else if (m_roomIndex == 1)
		m_nextRoomIndex = 0;
	*/

	const b3Body * world = RipExtern::g_world->getBodyList();
	u32 wc = RipExtern::g_world->GetBodyCount();
	//if (current > 0)
	//	current = 0;
	//m_rooms.at(current)->loadTextures();
	//int x = m_rooms.at(current)->getRoomIndex();
	//if(m_rooms.at(current)->getRoomIndex() != -1)
	//m_rooms.at(current)->LoadRoomToMemory();

	wc = RipExtern::g_world->GetBodyCount();

	//if ((current + 2) < m_rooms.size())
	//{
	//	//m_rooms.at(current + 2)->UnloadRoomFromMemory();

	//	m_unloadMutex.lock();
	//	m_unloadingQueue.push_back(current + 2);
	//	m_unloadMutex.unlock();
	//}
	//for (unsigned int i = 0; i < m_loadingQueue.size(); i++)
	//	m_rooms.at(m_loadingQueue.at(i))->loadTextures();

	//future = std::async(std::launch::async, &LevelHandler::_RoomLoadingThreading, this);
	
	//m_loadingQueue.clear();

}

void LevelHandler::_RoomLoadingThreading()
{
	for (unsigned int i = 0; i < m_loadingQueue.size(); i++)
	{
		//m_rooms.at(m_loadingQueue.at(i))->LoadRoomToMemory();
	}
	m_loadingQueue.clear();
	for (unsigned int i = 0; i < m_unloadingQueue.size(); i++)
	{
		//m_rooms.at(m_unloadingQueue.at(i))->UnloadRoomFromMemory();
	}
	m_unloadingQueue.clear();
}

