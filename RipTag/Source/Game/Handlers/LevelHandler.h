#pragma once
#include <filesystem>
#include <iostream>
#include <future>
#include "../Level/RoomGenerator.h"
class Room;
class Player;
class RoomGenerator;
class LevelHandler
{
private:
	RoomGenerator m_roomGenerator;

	const std::string m_roomString;
	std::vector<std::string> m_prefabRoomFiles;

	short unsigned int m_activeRoom;
	std::vector<Room*> m_rooms; //Released

	bool pressed = false; 

	Player * m_playerPtr; //Released in playstate
	b3World * m_worldPtr; //static in playsatet

	unsigned short m_roomIndex;
	unsigned short m_nextRoomIndex = 0;

private:
	std::vector<int> m_unloadingQueue;
	std::mutex m_unloadMutex;
	std::vector<int> m_loadingQueue;
	std::mutex m_loadMutex;

	std::future<void> future;
public:
	LevelHandler(const unsigned short & roomIndex);
	~LevelHandler();

	void Init(b3World & worldPtr, Player * playerPtr, const int & seed = 0, const int & roomIndex = 0);
	void Release();

	void Update(float deltaTime, Camera * camera);

	void Draw();

	void setPlayer(Player * playerPtr);

	const std::vector<Enemy*>* getEnemies() const;

	TriggerHandler * getTriggerHandler();

	std::tuple<DirectX::XMFLOAT4, DirectX::XMFLOAT4> getStartingPositions();

	const unsigned short getNextRoom() const;
private:
	void _LoadCorrectRoom(const int & seed, const int & roomIndex);

	void _GenerateLevelStruct(const int seed, const int amountOfRooms = 5);

	void _RoomLoadingManager(short int room = -1);
	void _RoomLoadingThreading();
};
